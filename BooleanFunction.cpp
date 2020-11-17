#include "BooleanFunction.h"
#include "PowerSet.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <sstream>

namespace BooleanFunction
{

    /***********************
     * 
     * Declarations
     * 
     ***********************/


    // returns true if the <term> is present in <terms>
    bool IsTermPresent(const std::set<std::string> term, const std::map<int, std::set<std::string> > terms);

    //splt string auxiliary function to parse networks
    std::vector<std::string> Split(const std::string &s, char delim);

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
    std::map<int, std::set<std::string> > Function::getTerms()
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

        std::map<int, std::set<std::string> > otherTerms = f->getTerms();

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
    std::vector<Function*> Function::getParents()
    {
        std::vector<Function*> result;

        int dimension = getDimension();

        if(dimension < 1)
            return result;

        BooleanFunction::PowerSet * power = new BooleanFunction::PowerSet(dimension);

        std::set< boost::dynamic_bitset<> > bitSet = convertToBitset();


        //Maximum Independent terms
        std::set< boost::dynamic_bitset<> > setIndependent = power->getMaxIndependent(bitSet);

        //rule1
        for(auto it = setIndependent.begin(), end = setIndependent.end(); it != end; it++)
        {
            std::set< boost::dynamic_bitset<> > newBitset = bitSet;
            newBitset.insert((*it));
            Function * f = convertToFunction(newBitset);
            result.push_back(f);
        }


        //rule2
        std::set< boost::dynamic_bitset<> > termParents = power->getAllParents(bitSet);
        termParents = power->filterContained(termParents, termParents, true);
        
        termParents = power->filterContained(termParents, setIndependent);

        //rule 2 + 3 add one or two terms if one is not possible and eliminate dominated terms
        for(auto it = termParents.begin(), end = termParents.end(); it != end; it++)
        {
            std::set< boost::dynamic_bitset<> > setCandidatesToAdd;
            setCandidatesToAdd.insert((*it));
            std::set< boost::dynamic_bitset<> > candidateFBitset = power->filterDominated(bitSet, setCandidatesToAdd);
            candidateFBitset.insert((*it));
            if(!power->isDegenerated(candidateFBitset))
            {
                //rule2
                Function * f = convertToFunction(candidateFBitset);
                result.push_back(f);
            }
            else
            {
                //rule3
                for(auto it2 = std::next(it); it2 != end; it2++)
                {
                    std::set< boost::dynamic_bitset<> > setCandidatesToAdd2 = setCandidatesToAdd;
                    setCandidatesToAdd2.insert((*it2));
                    std::set< boost::dynamic_bitset<> > candidateFBitset2 = power->filterDominated(bitSet, setCandidatesToAdd2);
                    candidateFBitset2.insert((*it));
                    candidateFBitset2.insert((*it2));
                    if(!power->isDegenerated(candidateFBitset2))
                    {
                        Function * f = convertToFunction(candidateFBitset2);
                        result.push_back(f);
                    }
                }
            }
        }

        return result;
    }


    // returns a vector of children of the function
    std::vector<Function*> Function::getChildren()
    {
        std::vector<Function*> result;

        int dimension = getDimension();

        if(dimension < 1)
            return result;

        BooleanFunction::PowerSet * power = new BooleanFunction::PowerSet(dimension);

        std::set< boost::dynamic_bitset<> > bitSet = convertToBitset();

        //vector of terms that do not respect rule 1 or rule 2 of computing children
        std::vector< boost::dynamic_bitset<> > termsNotR1R2;

        for(auto it = bitSet.begin(), end = bitSet.end(); it != end; it++)
        {
            std::set< boost::dynamic_bitset<> > candBitSet = bitSet;
            // S \ {c}
            candBitSet.erase((*it));

            //set of terms independent of S \ {c}
            std::set< boost::dynamic_bitset<> > setIndependents = power->getIndependent(candBitSet);

            std::set<  boost::dynamic_bitset<> > c;
            c.insert((*it));
            
            //TODO improve performance by doing the reverse
            // take all the children of the term removed and check if the term is independent
            //if it is add to the set of independents
            //filter independents not dominated by c
            setIndependents = power->filterNonDominated(setIndependents, c, true);

            //rule 1
            if(setIndependents.empty())
            {
                if(!power->isDegenerated(candBitSet))
                {
                    Function * f = convertToFunction(candBitSet);
                    result.push_back(f);
                }
                else
                {
                    //candidate for rule 3
                    termsNotR1R2.push_back((*it));

                }
            }
            //rule 2
            else
            {
                candBitSet.insert(setIndependents.begin(), setIndependents.end());
                Function * f = convertToFunction(candBitSet);
                result.push_back(f);

            }

        }

        //rule 3
        for(auto it1 = termsNotR1R2.begin(), end = termsNotR1R2.end(); it1 != end; it1++)
        {
            for(auto it2 = std::next(it1); it2 != end; it2++)
            {

                boost::dynamic_bitset<> intersectionBitSet = (*it1) & (*it2);
                //bug??
                if(true || intersectionBitSet.any())
                {
                    std::set< boost::dynamic_bitset<> > candBitSet = bitSet;
                    // S \ {c,c'}
                    candBitSet.erase((*it1));
                    candBitSet.erase((*it2));

                    boost::dynamic_bitset<> unionBitSet = (*it1) | (*it2);
                    candBitSet.insert(unionBitSet);

                    Function * f = convertToFunction(candBitSet);
                    result.push_back(f);

                }

            }
        }

        return result;
    }

    std::set< boost::dynamic_bitset<> > Function::convertToBitset()
    {
        std::set< boost::dynamic_bitset<> > result;
        int dimension = getDimension();

        std::map< std::string , int > varMap = getVariableMap();

        for(auto it = terms_.begin(), end = terms_.end(); it != end; it++)
        {
            std::set< std::string > term = it->second;

            boost::dynamic_bitset<> bs(dimension, 0);

            for(auto it2 = term.begin(), end2 = term.end(); it2 != end2; it2++)
            {
                auto id = varMap.find((*it2));
                if(id != varMap.end())
                {
                    bs.set(id->second - 1);
                }
            }
            if(bs.any())
            {
                result.insert(bs);
            }

        }

        return result;

    }

    Function * Function::convertToFunction(std::set< boost::dynamic_bitset<> > bitset)
    {

        Function * result = new Function(outputVariable_);

        std::map<int, std::string> varMap = getVariableMapById();

        //id of the term in dnf form
        int index = 1;

        for(auto it = bitset.begin(), end = bitset.end(); it != end; it++)
        {
            boost::dynamic_bitset<> bs = (*it);
            int dimension = bs.size();
            for(int i = 0; i < dimension; i++)
            {
                if(bs.test(i))
                {
                    auto var = varMap.find(i+1);
                    if(var != varMap.end())
                    {
                        result->addVariableToTerm(index, var->second);
                    }
                }
            }
            index++;
        }
        return result;

    }


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
        std::map<int, std::set<std::string> > terms = function->getTerms();
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


    bool IsTermPresent(const std::set<std::string> term, const std::map<int, std::set<std::string> > terms)
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

    Function * ParseFunction(std::string functionText)
    {
        Function * f = new Function();

        functionText.erase(std::remove_if(functionText.begin(),functionText.end(),isspace),functionText.end());

        //compact format -> {{1,2},{3,4}}
        // non compact format -> (( 1 && 2 ) || (3 && 4))
        bool isCompactFormat = false;
        if(functionText.find("{") != std::string::npos)
            isCompactFormat = true;

        if(functionText.length() < 3)
            return f; //TODO or NULL

        functionText = functionText.substr(1,functionText.length() - 2);

        std::vector<std::string> terms = Split(functionText, isCompactFormat ? '{' : '(');

        if(terms.size() < 2)
            return f; //TODO or NULL

        for(int i = 1; i < terms.size(); i++)
        {
            terms[i] = Split(terms[i], isCompactFormat ? '}' : ')')[0];

            std::vector<std::string> vars = Split(terms[i], isCompactFormat ? ',' : '&');

            for(int j = 0; j < vars.size(); j++)
            {
                if(vars[j].length()>0) //turn around for dounble &&
                {
                    f->addVariableToTerm(i, vars[j]);
                }
            }


        }

        return f;
    }

    std::vector<std::string> Split(const std::string &s, char delim)
    {
        std::vector<std::string> elems;
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
}