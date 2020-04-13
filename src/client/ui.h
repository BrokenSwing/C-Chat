/**
 * \brief Get the entered message by the user.
 *
 * The user input returned in buffer does not contain an ending carriage return.
 *
 * \param buffer Contains the entered message
 * \param buffer_size The size for the given buffer
*/
void ui_getUserInput(char* buffer, int buffer_size);

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