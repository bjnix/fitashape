//DGR_framework.h
#ifndef DGR_FRAMEWORK_H
#define DGR_FRAMEWORK_H

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

#define RELAY_LISTEN_PORT 25885
#define SLAVE_LISTEN_PORT 25884
#define BUFLEN 512


class MapNodePtr{
public:
	std::string name;
    size_t dataLength;
    virtual char * getDataString() =0;
    virtual void setData(char *) =0;
    MapNodePtr(std::string n) : name(n){}
    MapNodePtr(std::string n, size_t dL) : name(n), dataLength(dL){}
};

//only works with POD types
template<typename T>
class MapNode : public MapNodePtr{

protected:
    T * data;

public:
    T * getData(){ 
        return &data;
    }

    char * getDataString(){
        char * data_array = new char[dataLength];
        memcpy(data_array, data, dataLength);        
        return data_array;
    }
    void setData(char * data_array){
        memcpy(data, data_array, dataLength);
        //std::cout<< data << std::endl;
    }

    MapNode(std::string n, T *d) : MapNodePtr(n), data(d){
        dataLength = sizeof(T);
    }
    MapNode(std::string n, T *d, size_t dL): MapNodePtr(n,dL), data(d){}
};

class DGR_framework{


private:

	enum type_enum{     //enumeration of data 
	
	    //
	    DGR_VOID,           //0 void
	    DGR_INT,            //1 int
	    DGR_DOUBLE,         //2 double
	    DGR_FLOAT,          //3 float
	    DGR_CHAR,           //4 char
	    DGR_BOOL,           //5 bool
	    //6->14 future use

	    //C arrays
	    DGR_PTR_VOID=16,    //16 void *
	    DGR_PTR_INT,        //17 int *
	    DGR_PTR_DOUBLE,     //18 double *
	    DGR_PTR_FLOAT,      //19 float *
	    DGR_PTR_CHAR,       //20 char *
	    DGR_PTR_BOOL,       //21 bool *
	    //16->30 future use ---------------------------
	    
	    //STL Containers
	    DGR_STD_STRING=32,      	//32 std::string

	    //example sequence containers
	    DGR_STD_ARRAY_int,			//33 std::array of ints
	    DGR_STD_VECTOR_float,       //34 std::vector of floats
	    DGR_STD_DEQUE_string,       //35 std::deque of strings
	    DGR_STD_FORWARD_LIST_bool,  //36 std::forward_list of booleans
	    DGR_STD_LIST_ptr_char,      //37 std::list of char*'s

	    //container adaptors
	    DGR_STD_STACK,          	//38 std::stack
	    DGR_STD_QUEUE,          	//39 std::queue
	    DGR_STD_PRIORITY_QUEUE, 	//40 std::priority_queue

	    //associative containers
	    DGR_STD_SET,            	//41 std::set
	    DGR_STD_MULTISET,       	//42 std::multiset
	    DGR_STD_MAP,            	//43 std::map
	    DGR_STD_MULTIMAP,       	//44 std::multimap

	    //Unordered associative containers
	    DGR_STD_UNORDERED_SET,      //45 std::unordered_set
	    DGR_STD_UNORDERED_MULTISET, //46 std::unordered_multiset
	    DGR_STD_UNORDERED_MAP,      //47 std::unordered_map
	    DGR_STD_UNORDERED_MULTIMAP, //48 std::unordered_multimap

	   
	    //50->127 future use ------------------------------------------------

	    //command section 128->256
	    DGR_PTR_FUNCTION = 128,          //128 function pointer for callbacks   
	    CMD_NEW_OBJECT

	};

	template<typename T>
	type_enum typeid_int(T data_typeid)
	{
	    if(typeid(void)     == typeid(data_typeid)) return DGR_VOID;
	    else if(typeid(int)      == typeid(data_typeid)) return DGR_INT;
	    else if(typeid(bool)     == typeid(data_typeid)) return DGR_DOUBLE;
	    else if(typeid(float)    == typeid(data_typeid)) return DGR_FLOAT;
	    else if(typeid(char)     == typeid(data_typeid)) return DGR_CHAR;
	    else if(typeid(bool)     == typeid(data_typeid)) return DGR_BOOL;

	    else if(typeid(void *)   == typeid(data_typeid)) return DGR_PTR_VOID;
	    else if(typeid(int *)    == typeid(data_typeid)) return DGR_PTR_INT;
	    else if(typeid(bool *)   == typeid(data_typeid)) return DGR_PTR_DOUBLE;
	    else if(typeid(float *)  == typeid(data_typeid)) return DGR_PTR_FLOAT;
	    else if(typeid(char *)   == typeid(data_typeid)) return DGR_PTR_CHAR;
	    else if(typeid(bool *)   == typeid(data_typeid)) return DGR_PTR_BOOL;
	    else
	    {
	        return DGR_VOID;
	    }
	}
	
	//TODO: For making new Nodes in realtime
	// int make_Int(char* data){}
	// char make_Char(char* data){}

	// void make_Obj(char* data, type_enum type_name) 
	// {

	//     switch(type_name){
	//     case DGR_INT:
	//     	make_Int(data);
	//     	break;
	//     case DGR_CHAR:
	//     	make_Char(data);
	//     	break;
	//     default:
	// 	}
	// }

	/**
	 * Name: MapNode
	 * Description: A template class for a map node
	 */



public:
	// Register a callback that is called when the program exits so we can be
	// sure to close the port we're using.
	// Exit with error message

	std::map<std::string,MapNodePtr *> * InpMap;
	
	DGR_framework(char* r_IP);
	DGR_framework();

	int * sokt;
	bool * recvPack;


	template<typename T>
	void addNode(std::string n, T *d, size_t dL){
		MapNode<T> * newNode = new MapNode<T>(n,d,dL);
		InpMap->insert(std::pair<std::string,MapNodePtr *>( n, (MapNodePtr*)newNode ) );
	}

	template<typename T>
	void addNode(std::string n, T *d ){
		MapNode<T> * newNode = new MapNode<T>(n,d);
		InpMap->insert(std::pair<std::string,MapNodePtr *>(n , (MapNodePtr*)newNode ) );
	}

	//TODO: For making new Nodes in realtime
	// void addNode(std::string n, char *d, typeid_int dT, size_t dL ){
	// 	InputMap.inert(std::pair<std::string,MapNodePtr *>(n,(MapNodePtr*)MapNode()));
	// }

};//end DGR_framework
#endif

