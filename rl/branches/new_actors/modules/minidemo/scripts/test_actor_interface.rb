# TODO Skript ausbauen in Hinblick auf Interaktion Held <-> Item.
# und Aktoren untereinander.
# Halt diese Klassen wie sie hier beschrieben sind auch benutzen.
# TODO �ber Speichersparen nachdenken. Inwiefern k�nnen Instanzen
# einiger Klassen f�r Instanzen anderer Klassen nachgenutzt werden?
# Inwiefern ist Deferred Construction sinnvoll?

# Die Aktion f�r das Anz�nden der Fackel.
class LightTorchAction < RubyAction
    def initialize
        super("Anz�nden", "Die Fackel anz�nden f�r mehr Licht und so.");
    end

    # Die Methode pr�ft, ob die Aktion �berhaupt angeboten wird.
    def canDo?(torch, user)
        # Wenn die Fackel aus ist, kann sie angemacht werden.
        not torch.isAttributeSet("lit");
    end

    def doAction(torch, user, target)
        # Fackel hat ein Licht am Slot SLOT_FAR_END
        # Man k�nnte es auch so machen, dass dieses Licht jetzt
        # erzeugt und angeheftet wird. Spr�che einiges daf�r.
        # Das alles ist implizites Wissen. Deshalb sollte es
        # allgemeine Richtlinien geben und die Item-Doku sollte
        # sowas ganz klar ausweisen. Da aber alles innerhalb von Ruby
        # geschieht, ist das nicht so tragisch. Die C++-Seite ist
        # an der Stelle nur dumm und macht was Ruby sagt.

        torchActor = torch.getActor();
        torchActor.getChildBySlotAndIndex(Slots.SLOT_FAR_END, 0).activate();
        torchActor.getChildByName("TorchSparks").activate();
        torchActor.getChildByName("TorchCrackle").activate();
        # Activation sollte folgenderma�en geregelt sein:
        #       ist Child activated, so wird er angezeigt/erklingt
        #       wenn Parent-Activation true ist, sonst nicht
        #       Parent-Activity-Flag wird nicht an die
        #       Childs weitergereicht.

        #TODO timer setzen, damit die Fackel nach Ablauf ihrer Lebensdauer
        # ausgeht.

        torch.setAttribute("lit", true);
    end
end

# TODO Physikalische Attribute etc..
# TODO Persistenz *schreck*
class Torch < RubyItem
    def initialize(name)
        # Momentan stelle ich mir die Erzeugung so vor:
        # Die Aktorerstellung �ber spezifische
        # Fabrikmethoden f�r die Dinge, die der Aktor repr�sentiert.
        # Die erzeugen alle die gleiche Aktorklasse,
        # aber erzeugen auch das Ogre::Movable
        # (bzw RlMovable, was das kapselt) und h�ngen es ran.

        # Argumente hier: Name, Mesh-Name, Die physikalische Form
        # und die Dichte (g/cm^3) des Objektes.
        torchActor = $ActorManager.createMeshActor("Torch", "Torch.mesh",
            Physics.CAPSULE_GEOM, 1.0);

        lightActor = $ActorManager.createLightActor("TorchLight",
            Color.YELLOW, Light.POINTLIGHT, Light.LINEAR_ATTENUATION);

        sparksActor = $ActorManager.createParticleActor("TorchSparks",
            "TorchSparks.particles");

        sfxActor = $ActorManager.createSoundActor("TorchCrackle",
            "TorchCrackle.wav", Sound.LOOP, Sound.VOLUME_QUIET);        

        # Defaultwert f�r den vierten Param ist Slots.SLOT_DEFAULT
        torchActor.attach(Slots.SLOT_FAR_END, lightActor);
        torchActor.attach(Slots.SLOT_FAR_END, sparksActor);
        torchActor.attach(Slots.SLOT_FAR_END, sfxActor);

        setName(name);
        setActor(torchActor);

        # F�gt ein Gegenstandsbschreibendes Attribut an.
        # Sollte man auch schauen, inwiefern sowas aus XMLs oder so
        # erzeugt werden kann.
        # Typ ist Boolean, Name ist "lit", Ausgangswert ist false.
        # Soll halt enhtalten, ob die Fackel brennt.
        addBooleanAttribute("lit", false);

        # RubyItem selbst impliziert ja schon ein paar Standardactions
        # (Aufnehmen, Ansehen, etc.)
        # TODO soll nicht so bleiben, dass jede Fackel ne separate
        # Action-Instanz erh�lt. Eine f�r alle sollte reichen.
        addAction(new LightTorchAction);
    end
end

torch = new Torch("Fackel des Grauens");
# Erst Place into Scene erzeugt nen SceneNode f�r den Actor und
# seine Childs. Gegenst�ck ist removeFromScene().
torch.getActor().placeIntoScene(position, orientierung);

