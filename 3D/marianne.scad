module base() {
 union() {
cube([39, 19, 3]);
translate ([39, 4, 0])
  cylinder (h = 3, r = 4, $fn = 64) ;
  
translate ([39, 15, 0])
cylinder (h = 3, r = 4, $fn = 64) ;
translate ([38, 4, 0]) 
cube([5, 11, 3]);

   // ce qui deviendra la languette
  translate([34, 6, 0])
   cube([5, 7, 4]);
 // ajout base support  
translate ([8, 9.5, 1])
   cylinder(h=4, r=3.5, $fn=32); // ce qui doit passer dans l'échancrure de la tôle
   translate ([8, 9.5, 4])
   cylinder(h=1, r=4.5, $fn=32); // la colerette qui retient le support sur la tôle
   translate ([8, 9.5, 4])
   cylinder(h=10, r1=3.5, r2=3, $fn=32); // la base pleine du support de roulette, épaisseur + 1 mm pour bien s'enfoncer dans la base et rayon à vérifier
translate ([8, 9.5, 13])
   cylinder(h=2.5, r=3.5, $fn=32); // le dessus du support de roulette, épaisseur + 1 mm pour bien s'enfoncer dans la base et rayon à vérifier
}
}

module vide() {
translate ([0.5, 0.5, 0])
  union() {
cube([38.5, 18, 0.5]);
translate ([38.5, 3.5, 0])
  cylinder (h = 0.5, r = 3.5, $fn = 64) ;
translate ([38.5, 14.5, 0])
cylinder (h = 0.5, r = 3.5, $fn = 64) ;
translate ([37.5, 3.5, 0]) 
cube([4.5, 11, 0.5]);
}
}




module chanfrein() {

    // Chanfrein côté Y = 0
  translate([0, -1, 0])
        rotate([45, 0, 0])
            cube([50, 5, 5]);

    // Chanfrein côté Y = l
   translate([0, 20, 0])
        rotate([45, 0, 0])
         cube([50, 5, 5]);

  // chanfrein x = 43
  translate([45, 0, 0]) 
  rotate([0, -65, 0])
  cube([10, 19, 10]);

// chanfrein congé
  translate([39, 15, 1])
  difference() {
cube([7.1, 7.1, 7.1]);
  cylinder(h = 2.1, r1 = 4, r2 = 2, $fn = 64);
  }

// chanfrein congé x = 43, y = 0
  translate([39, 4.1, 1])
  difference() {
translate([0, -7.1, 0])
  cube([7.1, 7.1, 6.1]);
  cylinder(h = 2.1, r1 = 4, r2 = 2, $fn = 64);
  }
}

module ouverture() {
translate([33, 6, -1])
union() {
  cylinder(h = 5, r = 1, $fn = 32);
translate([0, 7, 0])
cylinder(h = 5, r = 1, $fn = 32);
translate([4, 7, 0])
cylinder(h = 5, r = 1, $fn = 32);
translate([4, 0, 0])
cylinder(h = 5, r = 1, $fn = 32);
translate([0, -1, 0])
cube([4, 2, 5]);
 translate([0, 6, 0])
cube([4, 2, 5]);
 translate([-1, 0, 0])
cube([2, 7, 5]);

  translate([9, -1, 0]) 
  rotate([0, -155, 0])
  cube([10, 9, 2]);
}
}

module vide_support() {
  translate ([8, 9.5, 5])
   cylinder(h=15, r=2, $fn=32); // le trou principal dans le support à roulette
translate([7.5, 0, 5])  
  cube([1, 19, 15]); // la fente du support à roulette
}

module plaque() {
difference() {
  base() ;
    vide();
chanfrein() ;
  ouverture();
  vide_support();
}
}
plaque() ;
