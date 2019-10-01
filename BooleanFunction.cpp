#include "BooleanFunction.h"

namespace BooleanFunction
{

    /***********************
     * 
     * Declarations
     * 
     ***********************/


    // returns true if the <term> is present in <terms>
    bool IsTermPresent(const std::set<std::string> term, const std::map<int, std::set<std::string>> terms);

    /***********************
     * 
     * Implementation
     * 
     ***********************/

    Function::Function() :
        variableMap_(std::map<std::string,int>()),
        variableMapById_(std::map<int,std::string>()),
        level_(std::vector<int>())
    {
        nTerms_ = 0;
    }
    
    Function::Function(std::string outputVariable) : Function::Function()
    {
        outputVariable_ = outputVariable;
    }

    Function::~Function(){}

    // sets the target output variable name
    void Function::setOutputVariable(std::string target)
    {
        outputVariable_ = target;
    }

    // returns the target output variable name
    std::string Function::getOutputVariable()
    {
        return outputVariable_;
    }

    // method to add a variable to a term
    void Function::addVariableToTerm(int termId, std::string variable)
    {
        if(termId > 0 && termId > nTerms_)
        {
            for(int i = nTerms_ + 1; i <= termId; i++)
            {
                std::set<std::string> term;
                terms_.insert(std::make_pair(i,term));
            }
            nTerms_ = termId;
        }
        if(termId > 0 && termId <= nTerms_)
        {
            auto it = terms_.find(termId);
            if(it != terms_.end())
            {
                it->second.insert(variable);
            }
        }
    }

    // returns a map with term identifier and the correspondent vector of variables
    std::map<int, std::set<std::string>> Function::getTerms()
    {
        return terms_;
    }

    int Function::getNTerms()
    {
        return nTerms_;
    }

    // returns the dimension of the Boolean function, i.e.
    // returns the number of variables it contains
    int Function::getDimension()
    {
        return (int) getVariableMap().size();
    }
    

    // returns a map between variable name and integer variable identifier
    std::map<std::string,int> Function::getVariableMap()
    {
        if(variableMap_.empty())
        {
            int index = 1;
            std::map<std::string,int> elements;
            std::map<int,std::string> elementsReversed;
            for(int i = 1; i <= nTerms_; i++)
            {
                std::set<std::string> term = terms_.find(i)->second;
                for(auto it = term.begin(), end=term.end(); it!=end; it++)
                {
                    auto ret = elements.insert(std::make_pair((*it),index));
                    if(ret.second != false)
                    {
                        elementsReversed.insert(std::make_pair(index, (*it)));
                        index++;
                    }
                }
            }
            variableMap_ = elements;
            variableMapById_ = elementsReversed;
        }
        return variableMap_;
    }

    // returns a map between variable integer identifier and its name
    std::map<int, std::string> Function::getVariableMapById()
    {
        if(variableMapById_.empty())
            getVariableMap();
        return variableMapById_;
    }

    // returns true if a given function f is equal to this function
    bool Function::isEqual(Function* f)
    {
        if(nTerms_ != f->getNTerms() || outputVariable_.compare(f->getOutputVariable()) != 0)
            return false;

        std::map<int, std::set<std::string>> otherTerms = f->getTerms();

        for(int i = 1; i <= nTerms_; i++)
        {
            //to prevent some function having duplicated clauses, we double check inclusiveness
            if(!IsTermPresent(terms_[i], otherTerms))
                return false;
            if(!IsTermPresent(otherTerms[i], terms_))
                return false;
        }
        
        return true;
    }

    // returns the level of the Boolean function
    std::vector<int> Function::getLevel()
    {
        if(level_.empty())
        {
            int nVar = getDimension();
            for(int i = 1; i <= nTerms_; i++)
            {
                std::set<std::string> term = terms_.find(i)->second;
                int count = (int)term.size();
                level_.push_back(nVar - count);
            }
            std::sort(level_.begin(), level_.end(), std::greater<int>());

        }
        return level_;
    }

    // compares the level of a given function with this function
    // returns
    //      -1 -> this function has a lower level
    //      0  -> this function has the same level
    //      1  -> this function has a greater level
    int Function::compareLevel(Function * f)
    {
        return compareLevel(f->getLevel());
    }
    int Function::compareLevel(std::vector<int> otherLevel)
    {
        std::vector<int> ownLevel = getLevel();

        int ownLevelSize = (int)ownLevel.size();
        int otherLevelSize = (int)otherLevel.size();

        int min = ownLevelSize;
        if(otherLevelSize < min)
        {
            min = otherLevelSize;
        }

        for(int i = 0; i < min; i++)
        {
            if(ownLevel[i] < otherLevel[i])
                return -1;
            if(ownLevel[i] > otherLevel[i])
                return 1;
        }

        if(ownLevelSize < otherLevelSize)
            return -1;
        if(ownLevelSize > otherLevelSize)
            return 1;
        return 0;
    }

    // returns a vector of parents of the function
    std::vector<Function*> getParents();

    // returns a vector of children of the function
    std::vector<Function*> getChildren();


    // returns a string representation of a function function
    std::string PrintFunctionLevel(Function *function)
    {
        std::string result = "";
        std::vector<int> level = function->getLevel();

        result += "(";
        for(int i = 0; i < function->getNTerms(); i++)
        {
            if(i > 0)
            {
                result += ",";
            }
            result += std::to_string(level[i]);
        }
        result += ")";

        return result;
    }


    std::string PrintFunction(Function *function)
    {
        std::string result = "";
        if(function->getNTerms() < 1)
        {
            result += "Empty Function";
        }
        std::map<int, std::set<std::string>> terms = function->getTerms();
        for(int i = 1; i <= function->getNTerms(); i++)
        {
            result += "(";
            std::set<std::string> term = terms[i];
            bool first = true;
            for(auto it = term.begin(), end = term.end(); it!=end; it++)
            {
                if(!first)
                {
                    result += " && ";
                }
                first = false;
                result += (*it);
            }
            result += ")";
            if(i < function->getNTerms())
            {
                result += " || ";
            }
        }
        return result;
    }


    bool IsTermPresent(const std::set<std::string> term, const std::map<int, std::set<std::string>> terms)
    {
        for(auto it = terms.begin(), end = terms.end(); it!=end; it++)
        {
            if(term.size() != it->second.size())
                continue;
            std::set<std::string> possMatch = it->second;
            bool isMatch = true;

            for(auto itElem = term.begin(), endElem = term.end(); itElem != endElem; itElem++)
            {
                std::set<std::string>::iterator itToFind;
                itToFind = possMatch.find((*itElem));
                if(itToFind == possMatch.end())
                {
                    isMatch = false;
                    break;
                }
            }

            if(isMatch)
                return true;

        }
        return false;
    }

}