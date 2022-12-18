#include <iostream>
#include <vector>
#include <set>
#include <map>

#include <cstdio>
#include <cmath>

using namespace std;

#define REPORT( X ) cout << #X << " = " << ( X ) << endl
#define REPORTP( P ) cout << #P << " = " << "(" << P.x << "," << P.y << ")" << endl


#define forn( X ) \
for ( int i = 0; i < ( X ); i++ )

#define x first
#define y second

typedef pair<int,int> pi;

// Apadted from
// Answer:  https://stackoverflow.com/a/21956177/18633613
// Post:    https://stackoverflow.com/questions/21956119/add-stdpair-with-operator
pi operator+(const pi& a, const pi& b) {
    return {a.x + b.x, a.y + b.y};
}

pi operator*(const int a, const pi& p) {
    return {a * p.x, a * p.y};
}

// Store offsets from the left (the bottom if there is a tie)

// ####
pi horizontal[] = { {0,0}, {1,0}, {2,0}, {3,0} };
int horizontalSize = sizeof( horizontal ) / sizeof( horizontal[ 0 ] );

// .#.
// ###
// .#.
pi plusShape[] = { {0,0}, {1,0}, {1,1}, {2,0}, {1,-1} };
int plusShapeSize = sizeof( plusShape ) / sizeof( plusShape[ 0 ] );

// ..#
// ..#
// ###
pi reverseL[] = { {0,0}, {1,0}, {2,0}, {2,1}, {2,2} };
int reverseLSize = sizeof( reverseL ) / sizeof( reverseL[ 0 ] );

// #
// #
// #
// #
pi vertical[] = { {0,0}, {0,1}, {0,2}, {0,3} };
int verticalSize = sizeof( vertical ) / sizeof( vertical[ 0 ] );

// ##
// ##
pi box[] = { {0,0}, {0,1}, {1,1}, {1,0} };
int boxSize = sizeof( box ) / sizeof( box[ 0 ] );

pi *shapes[] = { horizontal, plusShape, reverseL, vertical, box };
int sizes[] = { horizontalSize, plusShapeSize, reverseLSize, verticalSize };

// Yes, this is the grid
// x = [0, 6]
// y = [0, inf)
map<pi,char> grid;

int height = 0;

string jetPattern;
int idx = 0;

static void printGrid()
{
  if ( grid.size() == 0 ) {
    cout << "Empty grid" << endl;
    return;
  }

  int minY = 0, maxY = INT_MIN;
  int minX = 0, maxX = 6;

  for ( auto itemPair : grid ) {
    pi point = itemPair.first;
    if ( point.y > maxY )
      maxY = point.y;
  }

  // Print numbers
  // REPORT( height );
  cout << "    ";
  for ( int i = minX; i <= maxX; i++ ) {
    cout << i;
  }
  cout << endl;
  for ( int j = maxY; j >= minY; j-- ) {
    printf( "%3d ", j );
    for ( int i = minX; i <= maxX; i++ ) {
      auto it = grid.find( { i, j } );
      if ( it == grid.end() )
        cout << '.';
      else {
        char ch = it->second;
        // if ( ch == '#' )
        //   ch = '.';
        cout << ch;
      }
    }
    cout << endl;
  }

  height = maxY + 1;
  REPORT( height );
}

static char jetDirection()
{
  char dir = jetPattern[ idx ];
  if ( ++idx == jetPattern.size() ) idx = 0; // quick mod
  return dir;
}

pi leftDiff = {-1,0};
pi rightDiff = {1,0};

pi down = {0,-1};

static pi jetMove( pi initial, pi *shape, int size )
{
  char dir = jetDirection();
  REPORT( dir );
  pi diff = dir == '>' ? rightDiff : leftDiff;

  for ( int i = 0; i < size; i++ ) {
    pi current = initial + shape[ i ] + diff;
    if ( current.x < 0 || current.x > 6 || grid.count( current ) > 0 )
      return {0,0};
  }

  // All good
  return diff;
}

static bool downMove( pi initial, pi *shape, int size )
{
  for ( int i = 0; i < size; i++ ) {
    pi current = initial + shape[ i ] + down;
    if ( current.y < 0 || grid.count( current ) > 0 )
      return false;
  }

  return true;
}

// Temporarily put, print, erase a shape
static void printGridTemp( pi initial, pi *shape, int size )
{
  REPORT( "printGridTemp" );
  for ( int i = 0; i < size; i++ )
    grid[ initial + shape[ i ] ] = '@';
  printGrid();
  for ( int i = 0; i < size; i++ )
    grid.erase( initial + shape[ i ] );
}

static void placeShape( int n )
{
  pi *shape = shapes[ n ];
  int size = sizes[ n ];

  // Assume worst case
  pi initial = { 2, height + 3 + 1 }; // one more for plusShape

  // Try to go down
  bool goDown = true;
  REPORT( "Assumption" );
  printGridTemp( initial, shape, size );
  while ( goDown ) {
    REPORT( goDown );
    for ( int i = 0; i < size; i++ ) {
      // Look ahead for 3 spaces
      pi current = initial + shape[ i ] + ( 3 + 1 ) * down;
      if ( current.y < 0 || grid.count( current ) > 0 ) {
        goDown = false;
        break;
      }
    }

    if ( goDown ) {
      initial = initial + down;
    }
  }

  REPORT( "Initial" );
  printGridTemp( initial, shape, size );

  // Now we determined initial location, let it fall
  bool station = false;
  while ( !station ) {
    initial = initial + jetMove( initial, shape, size );
    // REPORT( "jetMove" );
    // printGridTemp( initial, shape, size );
    bool canDown;
    if ( ( canDown = downMove( initial, shape, size ) ) ) {
      initial = initial + down;
      // REPORT( "downMove" );
      // printGridTemp( initial, shape, size );
    } else {
      station = true;
    }
  }

  // REPORT( "Final" );
  // printGridTemp( initial, shape, size );

  for ( int i = 0; i < size; i++ ) {
    grid[ initial + shape[ i ] ] = '#';
  }
}

int main( int argc, char *argv[] )
{
  if ( argc != 2 ) {
    cerr << "Usage: day17 n" << endl;
    exit( 1 );
  }
  int n = atoi( argv[ 1 ] );
  cin >> jetPattern;
  
  forn( n ) {
    REPORT( i + 1 );
    placeShape( i % 5 );
    REPORT( "Done" );
    printGrid();
    cout << endl;
  }
}
