// bouton poussoir de fortune pour remplacer l'appui en caoutchouc
// déchiré/tombé d'une coque de clé Dacia à 2 trous (modèle années 2010)

module o(re, ri, a0, da) { // secteur "annulaire"
    rotate([0,0,a0]) translate([0,0,-.5]) 
    rotate_extrude(angle=da, $fn=60) translate([ri,0]) square([re-ri,9]);
}
// pour assitance au dimensionnement des ponts entre anneaux
function da(arc,r) = 180*arc/(PI*r);

module triple(d) { // pour fabriquer 3 exemplaires à la fois
    translate([0,-.43*d,0]) children();
    translate([-.5*d,.43*d,0]) children();
    translate([.5*d,.43*d,0]) children();   }

dbase=12.8; ebase=1; // base
dreb=10.2; ereb=0.5; hreb=1.5; // rebord : dreb = dext = din trou boitier
hcon=2.8;
espi=1.2;
lv=0.4; lp=1.8; // largeurs vides et pleins de la "spirale"

triple(dbase+.4) {

difference() {

union() {
linear_extrude(height = ebase) difference() { // assise périphérique
    circle(d=dbase, $fn=60); circle(d=dreb-2*ereb, $fn=60); }
linear_extrude(height = ebase+hreb) difference() {
    circle(d=dreb, $fn=60); circle(d=dreb-2*ereb, $fn=60); } // rebord
linear_extrude(height = espi) circle(d=dreb, $fn=60); // disque à spirale
linear_extrude(height = ebase+hcon) circle(2, $fn=60) ; // contact
rotate([0,0,-25]) translate([0,0,espi]) // renfort attache spirale
    rotate_extrude(angle=25, $fn=60) translate([rr[0]-1,0])
        polygon([[0,0],[1,0],[1,1]]);
rotate([0,0,10]) translate([0,0,espi]) // renfort attache spirale
    rotate_extrude(angle=43, $fn=60) translate([rr[3],0])
        polygon([[0,0],[1,0],[0,1]]);
}

drr=[ 0 , lv , lv+lp , 2*lv+lp  ]; // cotes radiales vides et pleins
rr = [for (i = [0:3]) dreb/2-ereb-drr[i]];
echo(da(lp,rr[1])); // pour réglage manuel angle suivant
o(rr[0], rr[1], 0, 360-25); // coupure circulaire longue
echo(da(lv,rr[2])); // pour réglage manuel angle suivant
o(rr[1]+.1, rr[2]-.01, 0, 10); // coupure radiale étroite
echo(da(lp,rr[2])); // pour réglage manuel angle suivant
o(rr[2], rr[3], 10+43, 360-43); // coupure circulaire

}

}