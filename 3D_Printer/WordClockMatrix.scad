//
//  WordClockMatrix.scad
//
//  Use this to print an LED grid for a Word Clock
//
//  V1.1    2019-04-08 kater

// Use one of the following sections, leave the other(s) commented out
// At the bottom, choose whether to print one single part or four quarter parts

// small RIBBA (23 cm, 13x13 LEDs, small cells)
frame = 230;    // frame size in mm (x and y)
numCells = 13;  // number of cells (x and y)
cell = 1000/60; // cell size in mm; LED strip has 60 LEDs per meter
wall = 0.5;     // wall thickness -- may have to be increased if your slicer omits walls!
height = 10;    // grid height in mm
foot = 4;       // foot size in mm (diamonds in grid junctions)
/*
// large RIBBA (50 cm, 12x12 LEDs, large cells)
frame = 500;    // frame size in mm (x and y)
numCells = 12;  // number of cells (x and y);
                // when printing quarters: number will be incremented to be odd
cell = 1000/30; // cell size in mm; LED strip has 30 LEDs per meter
wall = 0.4;     // wall thickness
height = 20;    // grid height in mm
foot = 4;       // foot size in mm (diamonds in grid junctions)
*/

margin = (frame - cell * numCells) / 2;
numQuarterCells = floor(numCells/2)+1;

module SingleMatrix(withFrame = false) {
    union() {
      if (withFrame) { // outer frame
        cube([frame, wall, height]);
        translate([0, frame-wall, 0])
            cube([frame, wall, height]);
        cube([wall, frame, height]);
        translate([frame-wall, 0, 0])
            cube([wall, frame-wall, height]);
      }
      for (i = [0:numCells]) {
        translate([margin + i * cell, 0, 0])
        {
            // Y direction walls |
            translate([-wall/2,0,0])
            cube([wall, 2 * margin + numCells * cell, height]);
            // feet
                for (j = [0:numCells])
                    translate([0, margin + j * cell,  wall/2]) rotate(45)
                        cube([foot, foot, wall], center=true);
        }
        // X direction walls –
        translate([0, margin + i * cell, 0])
            translate([0,-wall/2,0])
            cube([2 * margin + numCells * cell, wall, height]);
      } // for()
      // support for freestanding walls
    }
}

module MatrixQuarter(withFrame = false) {
  translate([-margin-(numQuarterCells-1/2)*cell, -margin - (numQuarterCells-1/2) * cell,0])
    union() {
      if (withFrame) { // outer frame
        cube([margin + numQuarterCells * cell - wall, wall, height]);
        cube([wall, margin + (numQuarterCells-1) * cell, height]);
      }
      for (i = [0:numQuarterCells-1]) {
        translate([margin + i * cell, 0, 0])
        {
            // Y direction walls |
            translate([-wall/2,0,0])
            cube([wall, margin + (numQuarterCells-1) * cell, height]);
            // feet
                for (j = [0:numQuarterCells-1])
                    translate([0, margin + j * cell,  wall/2]) rotate(45)
                        cube([foot, foot, wall], center=true);
        }
        // X direction walls –
        translate([0, margin + i * cell, 0])
            difference() {
            translate([0,-wall/2,0])
            cube([margin + numQuarterCells * cell, wall, height]);
                translate([margin + numQuarterCells * cell - foot, -wall, -wall])
                cube([2*foot, 2*foot, , 3*wall]);
            }
        // wall receptors
        factor = 2.2;
        translate([margin + i * cell, margin + (numQuarterCells-1) * cell, 0])
          union() {
            // walls
            translate([-wall * factor, 0, 0])
              cube([wall, foot-1, height]);
            translate([+wall * factor, 0, 0])
              cube([wall, foot-1, height]);
            // funnel  
            translate([-wall * factor + .13, foot - 1.3, 0])
              rotate(45)
              cube([wall, 1.4, height]);
            translate([+wall * factor - .02, foot - 1.0, 0])
              rotate(-45)
              cube([wall, 1.4, height]);
            // foot under receptor
            translate([-foot/2, 0, 0])
              cube([1*foot, foot, wall]);
          }
      } // for()
      // support for freestanding walls
      translate([margin + numQuarterCells * cell - foot, 0, 0])
        for (j = [1:3])
            translate([j, 0, 0])
                cube([0.5, numQuarterCells * cell - foot - margin, wall*2]);
    }
}

module CompoundMatrix(dist = 5, withFrame = false) {
    for(alpha = [0:90:270]) {
        rotate(alpha)
            translate([-dist,-dist,0])
            MatrixQuarter(withFrame);
    }
}

// this will print a test piece to try the best gap value for the wall receptors
module FitTest() {
    num = 10;
    dist = 10;
    cube([(num+1)*dist, cell/3, wall]);
    cube([(num+1)*dist, wall, height]);
      for (i = [0:num]) {
        factor = 1 + (i+2)*0.1;
        echo(factor);
        translate([dist/2 + i * dist, 0, 0])
          union() {
            translate([-wall * factor, 0, 0])
              cube([wall, cell / 3, height]);
            translate([+wall * factor, 0, 0])
            cube([wall, cell / 3, height]);
          }
      }
      
  // => optimal factor: 2.2 <=> total gap = 2 * wall * 2.2 = 1.76
}

//FitTest();
//SingleMatrix();
//CompoundMatrix(dist=10);     // dist=0 to view assembled quarters -- not for printing!
MatrixQuarter();
