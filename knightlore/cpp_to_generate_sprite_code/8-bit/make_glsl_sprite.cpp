#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>

int main(int argc, char** argv)
{
  std::cout << "Make GLSL code to create a sprite.\nUsage: " << argv[0] << " <file.txt>\nFile should consist of spaces and non-space characters to represent pixels.\n";

  if (argc < 2)
    return 1;

  std::ifstream ifs;
  ifs.open(argv[1]);
  if (!ifs.is_open())
  {
    std::cout << "Failed to open " << argv[1] << ".\n";
    return 1;   
  }

  std::string line;
  std::deque< std::deque<int> > bytes;
  int maxBytesInLine = 0;
  while (std::getline(ifs, line))
  {
    std::cout << "Processing line: \"" << line << "\"\n";

    int bytesThisLine = 0;
    std::deque<int> bytesInLine;
    while (line.size())
    {
      bytesThisLine++;
      if (line.size() < 8)
      {
        line += std::string(8 - line.size(), ' ');
      }
      std::string byte = line.substr(0, 8);
      line = line.substr(8);

      int b = 0;
      for (int i = 0; i < 8; i++)
      {
        if (byte[i] != ' ')
          b += (int)pow(2, 7 - i);
      }
      std::cout << "  Byte: \"" << byte << "\" = " << b << "\n";
      bytesInLine.push_front(b);
    }
    std::cout << "Num bytes in this line: " << bytesThisLine << "\n";
    maxBytesInLine = std::max(maxBytesInLine, bytesThisLine);
    bytes.push_front(bytesInLine);
  }
 
  // Write out the GLSL function
  std::cout << "**** GLSL function follows ****\n\n";

  std::cout << "float spriteData(float a)\n{\n    float r = 0.;\n";
  for (int row = 0; row < bytes.size(); row++)
  {
    auto& rowBytes = bytes[row];
    while (rowBytes.size() < maxBytesInLine)
    {
      rowBytes.push_front(0);
    }

/*
    std::string expr;
    for (int col = 0; col < rowBytes.size(); col++)
    {
      int a = ((bytes.size()) - row - 1) * 16 + maxBytesInLine - col - 1;
      int b = bytes[row][col]; 

      if (col == 0)
        expr = ((std::stringstream&)(std::stringstream() << "mix(r, " << (int)b << "., step(a, " << (int)a << ".5))")).str();
      else
        expr = std::string("mix(") + expr + std::string("), ") + ((std::stringstream&)(std::stringstream()<< b << "., step(a, " << a << ".5)")).str(); 
 
      //std::cout << "    r = mix(r, " << b << "., step(a, " << a << ".5));\n";
    }
    std::cout << "    r = " << expr << ";\n";
  }
  std::cout << "    return r;\n}\n";
*/
 
    for (int col = 0; col < rowBytes.size(); col++)
    {
      int a = ((bytes.size()) - row - 1) * 16 + maxBytesInLine - col - 1;
      int b = bytes[row][col]; 

      std::cout << "    r = mix(r, " << b << "., step(a, " << a << ".5));\n";
    }
  }
  std::cout << "    return r;\n}\n";
 
  return 0;
}

