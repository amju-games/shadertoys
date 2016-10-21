//------------------------------------------------------------------------
// GLSL Sprite code generator
// Jason Colman Feb 2016
//------------------------------------------------------------------------
// Build on mac: clang make_glsl_sprite.cpp -lstdc++
// Outputs a GLSL function which returns pixel data for an address.
// The input is a text file where non-space characters are converted
//  to '1' bits in the output pixel data, and spaces are '0's.

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>

const int BITS_PER_BYTE = 31;
const int SCREEN_WIDTH = 16;
const int MAX_CHAINED_TENARY_OPS = 1;

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

  // Using deque so we can push_front -- another option would be to
  //  use a vector and reverse it once populated

  std::string line;
  // Each line in the file has a deque of bytes, 1 bit per pixel. 
  std::deque< std::deque<uint> > bytes;
  int maxBytesInLine = 0;
  while (std::getline(ifs, line))
  {
    std::cout << "Processing line: \"" << line << "\"\n";

    int bytesThisLine = 0;
    std::deque<uint> bytesInLine;
    while (line.size())
    {
      bytesThisLine++;
      if (line.size() < BITS_PER_BYTE)
      {
        line += std::string(BITS_PER_BYTE - line.size(), ' ');
      }
      std::string byte = line.substr(0, BITS_PER_BYTE);
      line = line.substr(BITS_PER_BYTE);

      uint b = 0;
      for (int i = 0; i < BITS_PER_BYTE; i++)
      {
        if (byte[i] != ' ')
        {
          uint bitval = (uint)pow(2, BITS_PER_BYTE - 1 - i);
          b += bitval;
          std::cout << "  Adding " << bitval << ", pos " << i << "\n";
        }
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

  std::cout << "int spriteData(int a)\n{\n";

  int size = bytes.size();
  if (size * maxBytesInLine < MAX_CHAINED_TENARY_OPS)
  {
    std::cout << "    return ";
    for (int row = size - 1; row >= 0; row--)
    {
      auto& rowBytes = bytes[row];
      while (rowBytes.size() < maxBytesInLine)
      {
        rowBytes.push_front(0);
      }

      for (int col = rowBytes.size() - 1; col >= 0; col--)
      {
        int a = bytes.size() - row - 1; 
        uint b = bytes[row][col]; 

        std::cout << "(a < " << a << ") ? " << b << " : ";
      }
    }
    std::cout << "0;\n}\n";
  }
  else 
  {
    // Whole lotta data, split up into multiple lines.
    // Need them in reverse order
    std::deque<std::string> strs;

    std::cout << "    int r = 0;\n";
    for (int row = size - 1; row >= 0; )
    {
      std::stringstream ss;
      ss << "    r = ";
      for (int t = 0; t < MAX_CHAINED_TENARY_OPS; t++)
      {
        auto& rowBytes = bytes[row];
        while (rowBytes.size() < maxBytesInLine)
        {
          rowBytes.push_front(0);
        }

        for (int col = rowBytes.size() - 1; col >= 0; col--)
        {
          int a = bytes.size() - row - 1;
          uint b = bytes[row][col]; 

          ss << "(a < " << a << ") ? " << b << " : ";
        }

        row--;
      }
      ss << "r;\n";
      strs.push_front(ss.str());
    }

    for (int i = 0; i < strs.size(); i++)
    {
      std::cout << strs[i];
    }

    std::cout << "    return r;\n}\n";
  }

  return 0;
}

