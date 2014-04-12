//DGR_framework.h
#ifndef _DGR_FRAMEWORK_H
#define _DGR_FRAMEWORK_H
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <typeinfo>
#include <iostream>

#define RELAY_LISTEN_PORT 25885	/**< default listening port for relay */
#define SLAVE_LISTEN_PORT 25884 /**< default listening port for slave */
#define BUFLEN 512				/**< DGR will split packets to be this size */


/** MapNodePtr Class
 *	@detail		Class that allows for us to have a multi-type map. Anything  
 * 				that goes into the input map must extend this class
 *	@attention 	contains virtual functions and is intended to be extended by
 *				other classes. Do not try to use without pointing to a MapNode
 */
class MapNodePtr{
public:
	std::string name; 	/**< name of data node */
    size_t dataLength;	/**< length of array holding serialized data */
    /** getDataString
	 *	@arg @c void
	 *	@return @c 	data_array : a raw char* array of the data 
	 *	@attention 	virtual function!
	 */
    virtual char * getDataString() =0;
    
    /** getDataString
	 *	@arg @c data_array : a raw char* array of the data
	 *	@return @c void 
	 *	@attention 	virtual function!
	 */ 
    
    virtual void setData(char *) =0;
    /** MapNodePtr(string)
	 *  @arg @c n : name of node
     *	@return @c 	void 
	 *	@attention	only use if you assign dataLength a value	
     */ 
    MapNodePtr(std::string n) : name(n){}
    MapNodePtr(std::string n, size_t dL) : name(n), dataLength(dL){}
};//end MapNodePtr Class

/**	MapNode Class
 *	@detail		Template that catalogues and serializes an object of any type
 *				as well as take in a raw char* representation of that data and
 *				parse it back into the correct format
 * 	@attention	only works with POD types without padded data (i.e no structs, 
 *				unions) if an object is not guaranteed to have contiguous data,
 * 				YOU MUST specialize the template in order to use it
 */
template<typename T>
class MapNode : public MapNodePtr{

protected:
    T * data;	/**< pointer to data */

public:
	/** getData()
	 *	@arg @c void
	 *	@return @c &data : a pointer to the node data 
	 */
    T * getData(){ 
        return &data;
    }//end getData()

	/** getDataString()
	 *	@arg @c void
	 *	@return @c 	data_array : a raw char* array of the data 
	 *	@attention 	One of the two methods (getDataString() and setData(char *) )
	 *				that need to be specialized if using custom types
	 */
    char * getDataString(){
        char * data_array = new char[dataLength];
        memcpy(data_array, data, dataLength);        
        return data_array;
    }//end getDataString()

    /** setData(char*)
	 *	@arg @c data_array : a raw char* array of the data
	 *	@return @c void 
	 *	@attention 	One of the two methods (getDataString() and setData(char *) )
	 *				that need to be specialized if using custom types
	 */
    void setData(char * data_array){
        memcpy(data, data_array, dataLength);
    }//end setData(char*)



	// == Ctors,Dtor ===================
    
    /** MapNode(string,T*) with implicit data length
     *	@arg @c n : name of node
     *	@arg @c d : pointer to the node data
	 *	@return @c 	void 
	 *	@attention	only use if you are sure that sizeof(datatype) will give an 
	 *				accurate size
     */
    MapNode(std::string n, T *d) : MapNodePtr(n), data(d){
        dataLength = sizeof(T);
    }//end MapNode(string,T*)
    

    /** MapNode(string,T*,size_t) with explicit data length
     *	@arg @c n : name of node
     *	@arg @c d : pointer to the node data
     *	@arg @c dL : length of array needed to hold serialized data 
	 *	@return @c 	void 
	 *	@attention	data length is NOT NECESSARILY the size of the object, it 
	 *				is the length of the array needed to hold the serialized data
	 */
    MapNode(std::string n, T *d, size_t dL): MapNodePtr(n,dL), data(d){

    }//end MapNode(string,T*,size_t)


    //end == Ctors,Dtor ===================

};//end MapNode class

/** DGR_framework Class
 *	A simple framework that provides lightweight distributed variable 
 *	management
 *
 */
class DGR_framework{

private:

	void slaveInit(); /**< slave specific initializations */
	std::map<std::string,MapNodePtr *> * InpMap; /**< pointer to Map holding the pointers to all the data */

public:	


	// == Ctors,Dtor ===================
	// == MASTER Ctors
	/** DGR_framework(char*)
	 *	@ arg @c r_IP : the IP address of the relay node
	 *	@attention use this in conjuction with a relay node
	 */
	DGR_framework(char* r_IP);
	
	// == SLAVE Ctors
	/** DGR_framework(int) slave with specific listening port
	 *	@arg @c s_listen_port : the listening port on the slave node to be 
	 *							opened
	 *	@attention use this if you are running multiple slaves on the same node
	 */
	DGR_framework(int s_listen_port);

	/** DGR_framework() slave with default (RELAY_LISTEN_PORT) listening port
	 *	@arg @c s_listen_port : the listening port on the slave node to be 
	 *							opened
	 *	@attention use this if you are running one slave on each node
	 */
	DGR_framework();
	
	// == Dtor
	~DGR_framework();	/**< closes socket */

	//end == Ctors,Dtor ===================


	/** addNode(string,T*) with implicit data length
	 *	@arg @c n : name of node
	 *	@arg @c d : pointer to the node data
     *	@return @c 	void 
	 *	@attention	only use if you are sure that sizeof(datatype) will give an 
	 *				accurate size
	 */
	template<typename T>
	void addNode(std::string n, T *d ){
		MapNode<T> * newNode = new MapNode<T>(n,d);
		InpMap->insert(std::pair<std::string,MapNodePtr *>(n , (MapNodePtr*)newNode ) );
	}//end addNode(string,T*)

	/** addNode(string,T*,size_t) with explicit data length
	 *	@arg @c n : name of node
	 *	@arg @c d : pointer to the node data
     *	@arg @c dL : length of array needed to hold serialized data 
	 *	@return @c 	void 
	 *	@attention	data length is NOT NECESSARILY the size of the object, it 
	 *				is the length of the array needed to hold the serialized data
	 
	 */
	template<typename T>
	void addNode(std::string n, T *d, size_t dL){
		MapNode<T> * newNode = new MapNode<T>(n,d,dL);
		InpMap->insert(std::pair<std::string,MapNodePtr *>( n, (MapNodePtr*)newNode ) );

	}//end addNode(string,T*,size_t)

};//end DGR_framework Class

#endif
