# TODO Skript ausbauen in Hinblick auf Interaktion Held <-> Item.
# und Aktoren untereinander.
# Halt diese Klassen wie sie hier beschrieben sind auch benutzen.
# TODO Über Speichersparen nachdenken. Inwiefern können Instanzen
# einiger Klassen für Instanzen anderer Klassen nachgenutzt werden?
# Inwiefern ist Deferred Construction sinnvoll?
require 'globals.rb'
require 'actions.rb'
require 'items.rb'

class OpenDoorAction < RubyAction
  def initialize
    super("opendoor", "Tür öffnen");
  end
  
  # Die Methode prüft, ob die Aktion überhaupt angeboten wird.
  def canDo(door, user)    
    not door.isOpen
  end
  
  def doAction(door, user, target)    
    doorActor = door.getActor(); 
    doorActor.getControlledObject().replaceAnimation("zu", "auf", 1.0, 1);
    knarzActor = doorActor.getChildByName(doorActor.getName()+"_knarzen");
    knarzActor.getControlledObject().play(0);
    door.setOpen(true);
  end
end

class CloseDoorAction < RubyAction
  def initialize
    super("closedoor", "Tür schließen");
  end
  
  # Die Methode prüft, ob die Aktion überhaupt angeboten wird.
  def canDo(door, user)    
    door.isOpen
  end
  
  def doAction(door, user, target)    
    doorActor = door.getActor();
    doorActor.getControlledObject.replaceAnimation("auf", "zu", 1.0, 1); 
    knarzActor = doorActor.getChildByName(doorActor.getName()+"_knarzen");
    knarzActor.getControlledObject().play(0);
    door.setOpen(false);
  end
end

class Door < RubyItem
  def initialize(name, isOpen, canBeOpened)
    super(10, name, "Eine Tuer");

    doorActor = $AM.createMeshActor( name, "arc_tuer_01.mesh",  0, 0.0 ); #PhysicsManager::GT_BOX , 6.0);
    $CORE.log("door.rb - Aktor erstellt.");
    setActor(doorActor);
    $CORE.log("door.rb - Aktor gesetzt");
    soundActor = $AM.createSoundSampleActor(name+"_knarzen","doorcreak.ogg");
    doorActor.attachToSlot(soundActor,"Bone01");
    $CORE.log("door.rb - Sound hinzugefuegt");
    
    setBool("open", isOpen);
    
    if (canBeOpened)
    	addAction(OpenDoorAction.new);
    	addAction(CloseDoorAction.new);
    	$CORE.log("door.rb - Aktionen hinzugefuegt.");
    else
    	addAction(OpenDoorAction.new, Action::ACT_DISABLED);
    	addAction(CloseDoorAction.new, Action::ACT_DISABLED);
    	$CORE.log("door.rb - Aktionen versteckt hinzugefuegt.");
    end
  end
  
  def setOpen(open)
    @mOpen = open
  end
  
  def isOpen()
    @mOpen
  end
end

