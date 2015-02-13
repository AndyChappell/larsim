/*!
 * Title:   MergeSimSources Utility Class
 * Author:  Wes Ketchum (wketchum@lanl.gov)
 *
 * Description: 
 * Class that merges different simulation sources together to created a combined sim list.
 * Typically just merges vectors/maps/etc together. But, if anything as a G4 trackID, applies
 * a user-defined offset to those IDs.
 *
*/

#include <algorithm>
#include <stdexcept>

#include "MergeSimSources.h"

void sim::MergeSimSourcesUtility::MergeSimChannels(std::vector<sim::SimChannel>& merged_vector,
						   const std::vector<sim::SimChannel>& input_vector,
						   size_t source_index)
{
  if(source_index >= fG4TrackIDOffsets.size())
    std::runtime_error("ERROR in MergeSimSourcesUtility: Source index out of range!");

  std::pair<int,int> range_trackID(std::numeric_limits<int>::max(),
				   std::numeric_limits<int>::min());
  
  for(auto const& simchannel : input_vector){
    std::vector<sim::SimChannel>::iterator it = std::find(merged_vector.begin(),merged_vector.end(),simchannel);

    if(it==merged_vector.end()){
      merged_vector.emplace_back(simchannel.Channel());
      it = merged_vector.end() - 1;
    }

    std::pair<int,int> thisrange = it->MergeSimChannel(simchannel,fG4TrackIDOffsets[source_index]);
    if(thisrange.first < range_trackID.first) range_trackID.first = thisrange.first;
    if(thisrange.second > range_trackID.second) range_trackID.second = thisrange.second;
  }

  UpdateG4TrackIDRange(range_trackID,source_index);
}

void sim::MergeSimSourcesUtility::MergeAuxDetSimChannels(std::vector<sim::AuxDetSimChannel>& merged_vector,
							 const std::vector<sim::AuxDetSimChannel>& input_vector,
							 size_t source_index)
{
  if(source_index >= fG4TrackIDOffsets.size())
    std::runtime_error("ERROR in MergeSimSourcesUtility: Source index out of range!");

  std::pair<int,int> range_trackID(std::numeric_limits<int>::max(),
				   std::numeric_limits<int>::min());
  
  for(auto const& simchannel : input_vector){
    std::vector<sim::AuxDetSimChannel>::iterator it = std::find(merged_vector.begin(),merged_vector.end(),simchannel);

    if(it==merged_vector.end()){
      merged_vector.emplace_back(simchannel.AuxDetID());
      it = merged_vector.end() - 1;
    }
    
    std::pair<int,int> thisrange = it->MergeAuxDetSimChannel(simchannel,fG4TrackIDOffsets[source_index]);
    if(thisrange.first < range_trackID.first) range_trackID.first = thisrange.first;
    if(thisrange.second > range_trackID.second) range_trackID.second = thisrange.second;
  }
  
  UpdateG4TrackIDRange(range_trackID,source_index);
}

void sim::MergeSimSourcesUtility::MergeSimPhotons( std::vector<sim::SimPhotons>& merged_vector,
						   const std::vector<sim::SimPhotons>& input_vector)
{
  for(auto const& simphotons : input_vector){
    std::vector<sim::SimPhotons>::iterator it = std::find(merged_vector.begin(),merged_vector.end(),simphotons);

    if(it==merged_vector.end()){
      merged_vector.emplace_back(simphotons.OpChannel());
      it = merged_vector.end() - 1;
    }

    *it += simphotons;
  }
}

void sim::MergeSimSourcesUtility::MergeSimPhotonsLite( std::vector<sim::SimPhotonsLite>& merged_vector,
						       const std::vector<sim::SimPhotonsLite>& input_vector)
{
  for(auto const& simphotons : input_vector){
    std::vector<sim::SimPhotonsLite>::iterator it = std::find(merged_vector.begin(),merged_vector.end(),simphotons);
    
    if(it==merged_vector.end()){
      merged_vector.emplace_back(simphotons.OpChannel);
      it = merged_vector.end() - 1;
    }
    
    *it += simphotons;
  }
}

void sim::MergeSimSourcesUtility::UpdateG4TrackIDRange(std::pair<int,int> newrange, size_t source_index)
{
  if(source_index >= fG4TrackIDOffsets.size())
    std::runtime_error("ERROR in MergeSimSourcesUtility: Source index out of range!");

  if( newrange.first >= fG4TrackIDRanges[source_index].first &&
      newrange.second <= fG4TrackIDRanges[source_index].second)
    return;
  
  for(size_t i=0; i<fG4TrackIDRanges.size(); i++){
    if(i==source_index) continue;

    if( (newrange.first >= fG4TrackIDRanges[i].first && newrange.first <= fG4TrackIDRanges[i].second) ||
	(newrange.second >= fG4TrackIDRanges[i].first && newrange.second <= fG4TrackIDRanges[i].second) )
      {
	throw std::runtime_error("ERROR in MergeSimSourcesUtility: Source trackIDs overlap!");
      }
  }

  if(newrange.first < fG4TrackIDRanges[source_index].first)
    fG4TrackIDRanges[source_index].first = newrange.first;
  if(newrange.second > fG4TrackIDRanges[source_index].second)
    fG4TrackIDRanges[source_index].second = newrange.second;
  

}