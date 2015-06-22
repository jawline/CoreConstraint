#ifndef _COLUMN_DEF_H_
#define _COLUMN_DEF_H_
#include <string>

namespace Simplex {
  class Column {
    private:
      std::string _name;
      bool _artificial;
    public:
      Column();
      Column(std::string const& name);
      Column(char const* name, size_t len);
      ~Column();
      
      /**
       * Set whether this variable is an artificial variable
       */
      void setArtificial(bool a);
      
      /**
       * Check if this variable is an artificial variable inserted to find a initial table.
       */
      bool isArtificial() const;
      
      /**
       * Get the column name
       */
      std::string getName() const;
  };
}

#endif //_COLUMN_DEF_H_
