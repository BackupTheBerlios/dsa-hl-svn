# Nur eingebettet starten
load( "embed.rb" );

#hier wird Sonja2 geladen, wenn in 51 der Druide und in 52 der Wolf tot ist
#zudem Musikwechsel, Questwechsel, alles was toll ist für das Ende.

load( "npc.rb" );

print( "Sonjversion Map2 wird geladen" );
print( "Definiere Listener" );
sonj = Creature.new("Sonj", "Sonj");
sonj.setActor($AM.createMeshActor("Sonj", "men_sonja.mesh", PhysicsManager::GT_BOX, 0.0));
sonj.getActor().placeIntoScene([2.70, -37.79, -23.48]);

bot = $DS.loadBot("Sonj", "sonja2.xml");
bot.setNonPlayerCharacter(sonj);
sonj.addAction(TalkAction.new("Sonj", "sonja2.xml"))
agent = $AI.createAgent(bot);
agent.addSteeringBehaviour(DefaultIdleBehaviour.new);
#agent.addSteeringBehaviour(AvoidObstaclesBehaviour.new);
agent.addSteeringBehaviour(DialogBehaviour.new($heroVehicle));

print( "Sonja version Map2 fertig geladen" );

#Musik
$AM.getActor("techdemoDruidenWald").getControlledObject().stop()
$AM.getActor("kampfMusik").getControlledObject().stop()
$AM.getActor("ruchinsRuhe").getControlledObject().play()

#Quest
RulesSubsystem.getSingleton().getQuestBook().getQuest("hauptquest51").setState(Quest::SUCCEEDED)
RulesSubsystem.getSingleton().getQuestBook().getQuest("hauptquest52").setState(Quest::SUCCEEDED)
RulesSubsystem.getSingleton().getQuestBook().getQuest("hauptquest6").setState(Quest::ASSIGNED)
RulesSubsystem.getSingleton().getQuestBook().getQuest("hauptquest6").setKnown(true)

#Die Welt wird grün
load("wald.rb");