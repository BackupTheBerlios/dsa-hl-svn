load 'embed.rb'

$CORE.log("creating dummy player for menu.")
player = GameObject.new(10001, "Blah", "Blubb");
# PlayerSettings.preparePlayer(player)
$CORE.log("done.")

$CORE.log("showMainMenu()..")
$UI.showMainMenu(player)
$CORE.log("done.")
