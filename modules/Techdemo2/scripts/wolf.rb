# das fiese W�lfilein nach drau�en setzen

#Model schonmal reinladen mit was anderem als dem Wolf, Spinne z.B.


#Tagebucheintr�ge bei T�tung einstellen
	questBook = RulesSubsystem.getSingleton().getQuestBook()
	questBook.addJournalEntry("Der alte Mann aus dem Wald II",
	"Geschafft! Ein gro�er Wolf hatte den Vermissten angefallen und ihn im Turm festgesetzt, wo ich ihn verwundet und fiebrig fand. Ich musste den Wolf t�ten und habe den alten Mann notd�rftig versorgt. Ich sollte nun ins Dorf zur�ckkehren und alles berichten, die Dorfbewohner k�nnen sich dann um alles weitere k�mmern, nun wo der Weg wieder frei ist.");

#Waffe wegstecken wenn Wolfi tot ist
# $AM.getActor("Held").getGameObject().detachWeapon();  

#am Ende sonja2.rb laden


