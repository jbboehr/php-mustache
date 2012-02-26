
#include "mustache.hpp"


Mustache::Mustache() {
  startSequence = "{{";
  stopSequence = "}}";
}

Mustache::~Mustache () {
  ;
}

void Mustache::setStartSequence(string start) {
  startSequence = start;
}

void Mustache::setStopSequence(string stop) {
  stopSequence = stop;
}

std::string Mustache::getStartSequence() {
  return startSequence;
}

string Mustache::getStopSequence() {
  return stopSequence;
}

string * Mustache::render(string * tmpl, HashTable * data_hash) {
  HashPosition data_pointer = NULL;
  zval **data_entry = NULL;
  long data_count;
  
  int data_curkey_ret;
  ulong data_curkey_npos;
  char *data_curkey;
  long data_curkey_len;
  char *data_curval;
  long data_curval_len;
  
  string * return_val = new string();
  
  std::list<int> tagPositions;
  
  data_count = zend_hash_num_elements(data_hash);
  zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
  while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
    /*
    data_curkey_ret = zend_hash_get_current_key_ex(data_hash, &data_curkey, &data_curkey_len, &data_curkey_npos, true, data_entry);
    if( data_curkey_ret == HASH_KEY_IS_LONG ) {
      data_curkey_len = sprintf(data_curkey, "%ld", data_curkey_npos);
    }
    switch( Z_TYPE_PP(data_entry) ) {
      case IS_LONG:
      case IS_DOUBLE:
      case IS_STRING:
            convert_to_string(*data_entry);
            data_curval = Z_STRVAL_PP(data_entry);
            data_curval_len = Z_STRLEN_PP(data_entry);
            arr.insert(std::pair<*std::string, *std::string>(
                new std::string(data_curkey, data_curkey_len),
                new std::string(data_curval, data_curval_len))
            );
            break;
      default:
            php_error(E_WARNING, "Invalid data type");
            //RETURN_FALSE;
            break;
    }
    */
    zend_hash_move_forward_ex(data_hash, &data_pointer);
  }
  
  return return_val;
}
