
#include "mustachec.hpp"

int main( int argc, char * argv[] )
{
  return jsonlint_main(argc, argv);
  
  /*
  int showUsage = 0;
  int c;
  char * inFile = NULL;
  char * outFile = NULL;
  
  char * inFileData = NULL;
  long int inFileSize = 0;
  FILE * inFileResource;
  size_t inFileResult;
  
  opterr = 0;

  while( (c = getopt(argc, argv, "hi:o:")) != -1 ) {
    switch( c ) {
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
    
  // Input file is required
  if( NULL == inFile || showUsage ) {
    fprintf(stdout, "Usage: mustachec -i <input file> -o <output file>\n");
    return 1;
  }

  // Open file
  inFileResource = fopen(inFile, "rb");
  if( inFileResource == NULL ) {
    fprintf(stderr, "Input file does not exist.\n");
    return 1;
  }

  // Get file size
  fseek(inFileResource, 0, SEEK_END);
  inFileSize = ftell(inFileResource);
  rewind(inFileResource);

  if( inFileSize <= 0 ) {
    fprintf(stderr, "Input file is empty.\n");
    fclose(inFileResource);
    return 1;
  }

  // Allocate
  inFileData = (char *) malloc(inFileSize);
  if( inFileData == NULL ) {
    fprintf(stderr, "Memory error\n");
    fclose(inFileResource);
    return 1;
  }

  // Read
  inFileResult = fread(inFileData, 1, inFileSize, inFileResource);
  if( inFileResult != inFileSize ) {
    fprintf(stderr, "Reading error\n");
    fclose(inFileResource);
    return 1;
  }
  
  
  
  
  

//    for (index = optind; index < argc; index++)
//      printf ("Non-option argument %s\n", argv[index]);
  printf("in = %s, out = %s, data = %s\n", inFile, outFile, inFileData);
  
  return 0;
  */
}
