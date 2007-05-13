print( "\n- Registering Globals" );

include RlScript

# Der GC reisst oft die Objekte ab
GC.disable

$CORE = CoreSubsystem.getSingleton();
$JS = JobScheduler.getSingleton();
$AM = ActorManager.getSingleton();
$PM = PhysicsManager.getSingleton();
$UI = UiSubsystem.getSingleton();
$DM = DsaManager.getSingleton();
$DS = DialogSubsystem.getSingleton();
$WF = WindowFactory.getSingleton();
$AI = AgentManager.getSingleton();
$WPGM = WayPointGraphManager.getSingleton();
$DVM = DebugVisualsManager.getSingleton();
$SCRIPT = ScriptSubsystem.getSingleton();
$IM = InputManager.getSingleton();

$GameEveMgr = GameEventManager.getSingleton();
$AnimMgr = AnimationManager.getSingleton();
$GOM = GameObjectManager.getSingleton();
$SM = SoundManager.getSingleton();

class HeroPosWriter
  def initialize()
	
  end
  def writePos()
	actor = $UI.getActiveCharacter().getActor();
	$SCRIPT.log("[ "+actor.getWorldPosition()[0].to_s+", "+actor.getWorldPosition()[1].to_s+", "+actor.getWorldPosition()[2].to_s+"]" );  
	$SCRIPT.log("[ [ "+actor.getWorldOrientation()[0][0].to_s+", "+actor.getWorldOrientation()[0][1].to_s+", "+actor.getWorldOrientation()[0][2].to_s+" ] , "+actor.getWorldOrientation()[1].to_s+"]") ; 
  end
end

$hpw = HeroPosWriter.new()
print "Aktuelle Spielerposition in Log schreiben mit '$hpw.writePos'"

require 'steering.rb'
require 'initialize_factories.rb'
require "questsound.rb"
