
#include "mustachec.hpp"

int mustache_data_from_json(MustacheData * node, json_val_t * element)
{
  int i;
  char buff[100];
  MustacheData * child = NULL;
  string * ckey;
  
  if( !element ) {
    fprintf(stderr, "error: no element in tree\n");
    return -1;
  }

  switch( element->type ) {
    case JSON_FALSE:
    case JSON_NULL:
      node->type = MUSTACHE_DATA_STRING;
      node->val = new string("");
      break;
    case JSON_TRUE:
      node->type = MUSTACHE_DATA_STRING;
      node->val = new string("true"); // Meh
      break;
    case JSON_INT:
    case JSON_STRING:
    case JSON_FLOAT:
      node->type = MUSTACHE_DATA_STRING;
      node->val = new string(element->u.data);
      break;
    case JSON_OBJECT_BEGIN:
      node->type = MUSTACHE_DATA_MAP;
      for (i = 0; i < element->length; i++) {
        ckey = new string(element->u.object[i]->key, element->u.object[i]->key_length);
        child = new MustacheData();
        mustache_data_from_json(child, element->u.object[i]->val);
        node->data.insert(pair<string,MustacheData*>(*ckey,child));
      }
      break;
    case JSON_ARRAY_BEGIN:
      node->type = MUSTACHE_DATA_LIST;
      for (i = 0; i < element->length; i++) {
        child = new MustacheData();
        mustache_data_from_json(child, element->u.array[i]);
        node->children.push_back(child);
      }
      break;
    default:
      break;
  }
  return 0;
}

int main( int argc, char * argv[] )
{
  //return jsonlint_main(argc, argv);
  
  int showUsage = 0;
  int c;
  int silent = 0;
  
  char * templateFile = NULL;
  char * templateFileData = NULL;
  long int templateFileSize = 0;
  FILE * templateFileResource;
  size_t templateFileResult;
  
  char * inFile = NULL;
  char * outFile = NULL;
  
  opterr = 0;

  while( (c = getopt(argc, argv, "hst:i:o:")) != -1 ) {
    switch( c ) {
      case 's':
        silent = 1;
        break;
      case 't':
        templateFile = optarg;
        break;
      case 'i':
        inFile = optarg;
        break;
      case 'o':
        outFile = optarg;
        break;
      case '?':
        if( optopt == 'i' || optopt == 'o' ) {
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        } else if( isprint(optopt) ) {
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        return 1;
        break;
      case 'h':
      default:
        showUsage = 1;
        break;
    }
  }
    
  // Template and input files are required
  if( NULL == inFile || NULL == templateFile || showUsage ) {
    fprintf(stdout, "Usage: mustachec -t <tempalte file> -i <data file> (-o <output file>)\n");
    return 1;
  }
  
  // Open file
  templateFileResource = fopen(templateFile, "rb");
  if( templateFileResource == NULL ) {
    fprintf(stderr, "Input file does not exist.\n");
    return 1;
  }

  // Get file size
  fseek(templateFileResource, 0, SEEK_END);
  templateFileSize = ftell(templateFileResource);
  rewind(templateFileResource);

  if( templateFileSize <= 0 ) {
    fprintf(stderr, "Input file is empty.\n");
    fclose(templateFileResource);
    return 1;
  }

  // Allocate
  templateFileData = (char *) malloc(templateFileSize);
  if( templateFileData == NULL ) {
    fprintf(stderr, "Memory error\n");
    fclose(templateFileResource);
    return 1;
  }

  // Read
  templateFileResult = fread(templateFileData, 1, templateFileSize, templateFileResource);
  if( templateFileResult != templateFileSize ) {
    fprintf(stderr, "Reading error\n");
    fclose(templateFileResource);
    return 1;
  }
  
  // Get json data
  int ret = 0;
  json_config config;
  char *output = "-";

  memset(&config, 0, sizeof(json_config));
  config.max_nesting = 0;
  config.max_data = 0;
  config.allow_c_comments = 1;
  config.allow_yaml_comments = 1;
  
  json_val_t *root_structure;
  ret = do_tree(&config, inFile, &root_structure);
  
  // Init mustache
  string templateStr(templateFileData);
  MustacheData * templateData;
  string * templateOutput;
  Mustache mustache;
  
  // Load mustache data
  templateData = new MustacheData();
  mustache_data_from_json(templateData, root_structure);
  
  // Render
  try {
    templateOutput = mustache.render(&templateStr, templateData);
  } catch( MustacheException& e ) {
    fprintf(stderr, "%s\n", e.what());
    return 1;
  }
  
  // Output
  if( !silent ) {
    fprintf(stdout, templateOutput->c_str());
  }
  return 0;
}
