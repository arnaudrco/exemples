module rambarde()
{
    difference() {     
        cylinder(h = 20, r = 20, center = true);     
      union() {
        translate([0,10,10] ) cube([50, 10, 10], center = true);
 cylinder(h = 50, r = 5, center = true);
      }
    }
}
rambarde();
// Arnaud RCO
