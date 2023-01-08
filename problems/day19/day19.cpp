#include <iostream>
#include <vector>
#include <set>
#include <map>

#include <cstdio>

using namespace std;

#define REPORT( X ) cout << #X << " = " << (X) << endl
// like echo -n
#define REPORTN( X ) cout << #X << " = " << ( X ) << ", "

#define REPORTE( E ) printf("{%d,%d,%d,%d}", E.ore, E.clay, E.obsidian, E.geode )

// 2 Elements and 1 int
#define REPORT3( E1, E2, X ) REPORTE( E1 ), cout << ", ", REPORTE( E2 ), cout << ", " << X << endl

#define MINUTES 24

typedef pair<int,int> pi;

typedef struct {
  int oreCost;
  int clayCost;
  // <ore, clay>
  pi obsidianCosts;
  // <ore, obsidian>
  pi geodeCosts;

  int maxOre;
} BluePrint;

vector<BluePrint> bps;

static void printBps( FILE *fp )
{
  int id = 0;
  for ( BluePrint bp : bps ) {
    id++;
    fprintf( fp, "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian. {%d}\n",
    id,
    bp.oreCost,
    bp.clayCost,
    bp.obsidianCosts.first,
    bp.obsidianCosts.second,
    bp.geodeCosts.first,
    bp.geodeCosts.second,
    bp.maxOre );
  }
}

typedef struct {
  int ore;
  int clay;
  int obsidian;
  int geode;
} Elements;

Elements operator+( const Elements& lhs, const Elements& rhs )
{
  return { lhs.ore + rhs.ore,
          lhs.clay + rhs.clay,
          lhs.obsidian + rhs.obsidian,
          lhs.geode + rhs.geode };
}

Elements& operator+=( Elements& lhs, const Elements& rhs )
{
  lhs = lhs + rhs;
  return lhs;
}

static int getTotal( const BluePrint& bp, Elements inventory, Elements rates, int minutesLeft )
{
  // REPORT( minutesLeft );
  // REPORT3( inventory, rates, minutesLeft );
  // REPORTN( inventory.ore ), REPORTN( rates.ore ), REPORT( minutesLeft );
  if ( minutesLeft == 0 )
    return inventory.geode;
  minutesLeft--;
  
  int total = 0;

  // Geode robot
  auto [geodeOre, geodeObsidian] = bp.geodeCosts;
  if ( inventory.ore > geodeOre && inventory.obsidian > geodeObsidian ) {
    // REPORTN( inventory.geode ), REPORT( minutesLeft );
    auto newInv = inventory + rates;
    auto newRates = rates;
    newInv.ore -= geodeOre;
    newInv.obsidian -= geodeObsidian;
    newRates.geode++;
    total = max( total, getTotal( bp, newInv, newRates, minutesLeft ) );
  }

  // Obsidian robot
  auto [obsidianOre, obsidianClay] = bp.obsidianCosts;
  if ( rates.obsidian < geodeObsidian && inventory.ore > obsidianOre && inventory.clay > obsidianClay ) {
    // cout << "Obsidian ", REPORT( minutesLeft );
    auto newInv = inventory + rates;
    auto newRates = rates;
    newInv.ore -= obsidianOre;
    newInv.clay -= obsidianClay;
    newRates.obsidian++;
    total = max( total, getTotal( bp, newInv, newRates, minutesLeft ) );
  }

  // Clay robot
  if ( rates.clay < bp.obsidianCosts.second && inventory.ore > bp.clayCost ) {
    // cout << "Clay ", REPORT( minutesLeft );
    auto newInv = inventory + rates;
    auto newRates = rates;
    newInv.ore -= bp.clayCost;
    newRates.clay++;
    total = max( total, getTotal( bp, newInv, newRates, minutesLeft ) );
  }
  
  // Ore robot
  // REPORT( inventory.ore );
  if ( rates.ore < bp.maxOre && inventory.ore > bp.oreCost ) {
    // cout << "Ore ", REPORT( minutesLeft );
    auto newInv = inventory + rates;
    auto newRates = rates;
    newInv.ore -= bp.oreCost;
    newRates.ore++;
    total = max( total, getTotal( bp, newInv, newRates, minutesLeft ) );
  }

  // Do nothing
  if ( total == 0 ) {
    auto newInv = inventory + rates;
    total = getTotal( bp, newInv, rates, minutesLeft );
  }


  return total;
}

int main()
{
  string line;
  while( getline( cin, line ) ) {
    // Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian.
    BluePrint bp;
    sscanf( line.c_str(), "Blueprint %*d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.",
    &bp.oreCost,
    &bp.clayCost,
    &bp.obsidianCosts.first,
    &bp.obsidianCosts.second,
    &bp.geodeCosts.first,
    &bp.geodeCosts.second );
    bp.maxOre = max( max( bp.oreCost, bp.clayCost ), max( bp.obsidianCosts.first, bp.geodeCosts.first ) );

    bps.push_back( bp );
  }

  fclose( fopen( "PARSE", "w" ) );
  FILE *out = fopen( "PARSE", "w" );
  printBps( out );
  fclose( out );

  int ret = 0;

  Elements inventory = {};
  Elements rates = { 1, 0, 0, 0 };
  inventory += rates;
  int i = 0;
  for ( const auto& bp : bps ) {
    i++;
    int total = getTotal( bp, inventory, rates, MINUTES );
    ret += i * total;
    cout << i << ": " << total << " (" << i * total << ")" << endl;
  }
  cout << ret << endl;
}
