# Nur eingebettet starten
load( "embed.rb" );

print( "Actor-Tests wird geladen" );

$World.setShowBoundingBoxes( true );

# Normales Erstellen & Einf�gen
$SCRIPT.log("Partikel Actor erstellen");
partikelDings = $AM.createParticleSystemActor("EinfachsePartikel", "PEExamples/ignifaxius" );
$SCRIPT.log("Partikel Actor in die Szene einf�gen");
partikelDings.placeIntoScene( 415.0, 5.0, -100.0, 1.0, 0.0, 0.0, 0.0);


# Ein Objekt an einen Bone anh�ngen
$SCRIPT.log("BefestigungsMesh  Actor erstellen");
befestigDings = $AM.createMeshActor("BefestigungsMesh", "ins_fackel_01.mesh" );
$SCRIPT.log("BefestigungsMesh2  Actor erstellen");
befestigDings2 = $AM.createMeshActor("BefestigungsMesh2", "ins_fackel_01.mesh" );

$SCRIPT.log("HalteMesh Actor erstellen");
halteDings = $AM.createMeshActor("HalteMesh", "held.mesh" );
$SCRIPT.log("HalteMesh Actor in die Szene einf�gen");
halteDings.placeIntoScene( 315.0, 5.0, -100.0, 1.0, 0.0, 0.0, 0.0);

$SCRIPT.log("Mesh an HalteMesh befestigen");
halteDings.attachToSlotAxisRot( befestigDings, "Bone15", "SLOT_HANDLE", [0.0, 0.0, 0.0], [ 1.0, 0.0, 0.0 ], 90.0 );
$SCRIPT.log("Mesh von HalteMesh abnehmen");
halteDings.detach( befestigDings );
$SCRIPT.log("Mesh an HalteMesh erneut befestigen");
halteDings.attachToSlotAxisRot( befestigDings, "Bone15", "SLOT_HANDLE", [0.0, 0.0, 0.0], [ 1.0, 0.0, 0.0 ], 90.0 );
$SCRIPT.log("Mesh an BefestigungsMesh befestigen");
befestigDings.attachToSlotAxisRot( befestigDings2, "SLOT_FAR_END", "SLOT_HANDLE", [0.0, 0.0, 0.0], [ 1.0, 0.0, 0.0 ], 120.0 );
$SCRIPT.log("Animation abspielen");
halteDings.getControlledObject().startAnimation( "gehloop" )

# Objekte �ber Nodes verkn�pfen
$SCRIPT.log("BefestigungsPartikel Actor erstellen");
befestigDingsP = $AM.createParticleSystemActor("BefestigungsPartikel", "PEExamples/ignifaxius" );

$SCRIPT.log("Leeren Actor erstellen");
leerDings = $AM.createEmptyActor("Leer");
$SCRIPT.log("Leeren Actor in die Szene einf�gen");
leerDings.placeIntoScene( 115.0, 5.0, -100.0, 1.0, 0.0, 0.0, 0.0);

$SCRIPT.log("Partikel an Leeren Actor befestigen");
leerDings.attach( befestigDingsP );
$SCRIPT.log("Partikel vom Leeren Actor abnehmen");
leerDings.detach( befestigDingsP );
$SCRIPT.log("Partikel erneut an Leeren Actor befestigen");
leerDings.attach( befestigDingsP );
$SCRIPT.log("Leeren Actor aus der Szene entfernen");
leerDings.removeFromScene();
$SCRIPT.log("Leeren Actor wieder in die Szene einf�gen");
leerDings.placeIntoScene( 115.0, 5.0, -100.0, 1.0, 0.0, 0.0, 0.0);

print( "Actor-Tests fertig geladen" );