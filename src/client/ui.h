/**
 * \brief Initialize required resources for UI.
 */
void ui_init();

/**
 * \brief Free allocated resources for UI.
 */
void ui_cleanUp();

/**
 * \brief Reset UI state
 *
 * To be called in case of ui_getUserInput getting interrupted.
 */
void ui_reset();

/**
 * \file ui.h
 * \brief API to communicate with user.
 *
 * This adds an abstraction between main program logic and the way messages
 * are displayed to client.
 */

/**
 * \brief Get the entered message by the user.
 *
 * The user input returned in buffer does not contain an ending carriage return.
 *
 * \param prompt Prompt message
 * \param buffer Contains the entered message
 * \param buffer_size The size for the given buffer
*/
void ui_getUserInput(const char* prompt, char* buffer, int buffer_size);

/**
 * \brief Displays the given message from the given sender to the user.
 *
 * \param sender The sender name to display
 * \param message The message to display to client
 */
void ui_messageReceived(const char* sender, const char* message);

/**
 * \brief Displays an information message to client
 *
 * \param message The information message to display to client
 */
void ui_informationMessage(const char* message);

/**
 * \brief Displays an error message to the client.
 *
 * \param message The error message to display to the client
 */
void ui_errorMessage(const char* message);

/**
 * \brief Displays a join message to the client.
 *
 * \param username The username of the client who joined
 */
void ui_joinMessage(const char* username);