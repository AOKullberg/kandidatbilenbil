/* FIL kommunikation_klient.hpp
 * Header för kommunikation med datorn.
 */
#ifndef kommunikation_klient_h
#define kommunikation_klient_h

#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <exception>

/* Enbart fördefinition av alla funktioner för kommunikation. 
 */
/*class net_except : public exception
 {
	public:
	net_except(const char* errMessage):errMessage_(errMessage) {}
	
	const char* what() const throw() { return errMessage_; }
	
	private:
	
	const char* errMessage_; 
 };
*/
sf::Socket::Status init_connection();
void send_image();
#endif
