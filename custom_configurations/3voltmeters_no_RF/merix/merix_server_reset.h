/**
   USE OF THIS SOFTWARE IS GOVERNED BY THE TERMS AND CONDITIONS
   OF THE LICENSE STATEMENT AND LIMITED WARRANTY FURNISHED WITH
   THE PRODUCT.
   <p/>
   IN PARTICULAR, YOU WILL INDEMNIFY AND HOLD B2N LTD., ITS
   RELATED COMPANIES AND ITS SUPPLIERS, HARMLESS FROM AND AGAINST ANY
   CLAIMS OR LIABILITIES ARISING OUT OF THE USE, REPRODUCTION, OR
   DISTRIBUTION OF YOUR PROGRAMS, INCLUDING ANY CLAIMS OR LIABILITIES
   ARISING OUT OF OR RESULTING FROM THE USE, MODIFICATION, OR
   DISTRIBUTION OF PROGRAMS OR FILES CREATED FROM, BASED ON, AND/OR
   DERIVED FROM THIS SOURCE CODE FILE.
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Call when reset button is pressed
//
// There are two type of reset supported - one is simple reset e.g. the client is already bound with the server sending reset
// and master reset - e.g. the client is bounded with different server. The master reset is enabled only if the client has not received a valid message 
// for 5 min from the server that it is bounded to. 

#if defined(MODULE_IS_SERVER)

inline void SERVER_RESET()
{
  RF_SEND_RESET_REQUEST();
  ID_REINIT();
  SERVER_STATE_INIT();
  SERVER_DISPLAY_RESET();
  SERVER_STORE_INIT();
  RF_REINIT();
}

#endif
