#ifndef OVERFLOWPALETTEBACKEND_H
#define OVERFLOWPALETTEBACKEND_H

#include "GamePanelConfig.h"


constexpr int INVALID_COLOR_INDEX = -1;

class OverflowPaletteBackEnd
{
public:
	static bool
		IsValidCoordinate(const int& InRow, const int& InCol, const GamePanelConfig& InConfig)
	{
		return InRow >= 0 && InRow < InConfig.Rows && InCol >= 0 && InCol < InConfig.Cols;
	}

	using IndexType        = int;
	using BFS_CallbackType = void(
		const int& InRow, const int& InCol, const IndexType& InGridIndex, const bool& bIsEndLevel
	);
	using BoardLevelListType = std::vector<std::vector<IndexType>>;

	static void BFS_Iterator(
		const GamePanelConfig& InConfig,
		const int& InStartRow,
		const int& InStartCol,
		BoardLevelListType& OutLevelList,
		BFS_CallbackType InCallbackFunc
	)
	{
		// Check adjacent cells (up, down, left, right)
		static const std::pair<int, int> directions[] = {
			{ -1, 0  },
            { 1,  0  },
            { 0,  -1 },
            { 0,  1  }
		};

		std::vector<IndexType> CurrentLevelNodes;
		std::vector<bool> SearchedIndex(InConfig.GameGridColors.size(), false);

		const int StartIndex          = InConfig.CoordinateToIndex(InStartRow, InStartCol);
		const int ReferenceColorIndex = InConfig.GameGridColors[StartIndex];

		CurrentLevelNodes.push_back(StartIndex);
		SearchedIndex[StartIndex] = true;

		while ( !CurrentLevelNodes.empty() )
		{
			// Process all nodes in current level
			for ( const auto& CurrentIndex : CurrentLevelNodes )
			{
				int CurrentRow, CurrentCol;
				InConfig.IndexToCoordinate(CurrentIndex, CurrentRow, CurrentCol);
				InCallbackFunc(
					CurrentRow, CurrentCol, CurrentIndex, CurrentLevelNodes.back() == CurrentIndex
				);
			}
			OutLevelList.push_back(CurrentLevelNodes);

			// Prepare next level nodes
			std::vector<IndexType> NextLevelNodes;

			for ( const auto& CurrentIndex : CurrentLevelNodes )
			{
				int CurrentRow, CurrentCol;
				InConfig.IndexToCoordinate(CurrentIndex, CurrentRow, CurrentCol);

				for ( const auto& [dr, dc] : directions )
				{
					const int NewRow = CurrentRow + dr;
					const int NewCol = CurrentCol + dc;

					// Out of grid bound
					if ( !IsValidCoordinate(NewRow, NewCol, InConfig) )
					{
						continue;
					}

					int NewIndex = InConfig.CoordinateToIndex(NewRow, NewCol);

					// [ Different color ] or [ Searched ] index should be ignored
					bool bIsSameColor = InConfig.GameGridColors[NewIndex] == ReferenceColorIndex;
					bool bIsSearched  = SearchedIndex[NewIndex];
					if ( bIsSearched || !bIsSameColor )
					{
						continue;
					}

					// Add to next level and mark as searched
					NextLevelNodes.push_back(NewIndex);
					SearchedIndex[NewIndex] = true;
				}
			}

			// Move to next level
			CurrentLevelNodes = std::move(NextLevelNodes);
		}
	}

	/**
	 * @brief Region growing algorithm (4 directions) to generate a random contour from a start
	 * coordinate.
	 *
	 * @param InConfig The config point that contains the game grid.
	 * @param InStartRow The start row index.
	 * @param InStartCol The start column index.
	 * @param InRandomGrowSize The max step of the random growth.
	 * @return std::vector<IndexType> The index list of element in the contour.
	 */
	static std::vector<IndexType> RandomContour(
		GamePanelConfig& InConfig,
		const int& InStartRow,
		const int& InStartCol,
		const int& InRandomGrowSize
	)
	{
		// Check adjacent cells (up, down, left, right)
		static const std::pair<int, int> DIRECTIONS[] = {
			{ -1, 0  },
			{ 1,  0  },
			{ 0,  -1 },
			{ 0,  1  }
		};

		// Initialize containers for region tracking and visited state
		std::vector<IndexType> RegionIndices;
		std::vector<IndexType> FrontierIndices;
		std::vector<bool> SearchedIndex(InConfig.GameGridColors.size(), false);

		// Add starting point
		const int StartIndex = InConfig.CoordinateToIndex(InStartRow, InStartCol);
		FrontierIndices.push_back(StartIndex);
		RegionIndices.push_back(StartIndex);
		SearchedIndex[StartIndex] = true;

		int GrowthSteps = 0;
		while ( GrowthSteps < InRandomGrowSize && !FrontierIndices.empty() )
		{
			// Randomly select a frontier node
			int RandomNodeIndex = rand() % FrontierIndices.size();
			int CurrentIndex    = FrontierIndices[RandomNodeIndex];

			// Convert index to grid coordinates
			int CurrentRow, CurrentCol;
			InConfig.IndexToCoordinate(CurrentIndex, CurrentRow, CurrentCol);

			// Find all valid adjacent moves that haven't been visited
			std::vector<std::pair<int, int>> ValidMoves;
			for ( const auto& [dr, dc] : DIRECTIONS )
			{
				const int NewRow = CurrentRow + dr;
				const int NewCol = CurrentCol + dc;

				if ( !IsValidCoordinate(NewRow, NewCol, InConfig) )
				{
					continue;
				}

				int NewIndex = InConfig.CoordinateToIndex(NewRow, NewCol);
				if ( SearchedIndex[NewIndex] )
				{
					continue;
				}

				ValidMoves.push_back({ NewRow, NewCol });
			}

			// If no valid moves, remove current node from frontier
			if ( ValidMoves.empty() )
			{
				FrontierIndices.erase(FrontierIndices.begin() + RandomNodeIndex);
				continue;
			}

			// Randomly select and add new point
			auto [NewRow, NewCol] = ValidMoves[rand() % ValidMoves.size()];
			int NewIndex          = InConfig.CoordinateToIndex(NewRow, NewCol);
			FrontierIndices.push_back(NewIndex);
			RegionIndices.push_back(NewIndex);
			SearchedIndex[NewIndex] = true;
			GrowthSteps++;
		}

		return RegionIndices;
	}
};

#endif // OVERFLOWPALETTEBACKEND_H
