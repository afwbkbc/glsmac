#include "Rules.h"

namespace task {
namespace mainmenu {

Rules::Rules( MainMenu* mainmenu )
	: SlidingMenu(
	mainmenu, "GAME RULES", {
		{
			"PLAY WITH STANDARD RULES", {
											CH( this ) {
												// TODO: faction select
												m_mainmenu->StartGame();
											}
										}
		},
		{
			"PLAY WITH CURRENT RULES",  {
											CH( this ) {
												// TODO: previous rules load
												m_mainmenu->StartGame();
											}
										}
		},
		{
			"CUSTOMIZE RULES",          {
											CH( this ) {
												// TODO: rules select
												MenuError();
											}
										}
		},
	}
) {}

}
}
