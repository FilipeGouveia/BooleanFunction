//#include "BooleanFunction.h"
#include "PowerSet.h"
#include "BooleanFunction.h"

//#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <set>

int main(int argc, char ** argv)
{
    int dimension = 4;

    BooleanFunction::PowerSet * power = new BooleanFunction::PowerSet(dimension);
    boost::dynamic_bitset<> _123(4);
    _123[0] = 1;
    _123[1] = 1;
    _123[2] = 1;

    boost::dynamic_bitset<> _24(4);
    _24[1] = 1;
    _24[3] = 1;

    std::set< boost::dynamic_bitset<> > res1 = power->getIndependent(_123);
    std::cout << "Termos independentes de 123\n";
    for(auto it = res1.begin(), end = res1.end(); it != end; it++)
    {
        std::cout << (*it) << "\n";
    }

    std::set< boost::dynamic_bitset<> > res2 = power->getIndependent(_24);
    std::cout << "\nTermos independentes de 24\n";
    for(auto it = res2.begin(), end = res2.end(); it != end; it++)
    {
        std::cout << (*it) << "\n";
    }

    std::set< boost::dynamic_bitset<> > l;
    l.insert(_123);
    l.insert(_24);
    std::set< boost::dynamic_bitset<> > res3 = power->getIndependent(l);
    std::cout << "\nTermos independentes de 123 e 24\n";
    for(auto it = res3.begin(), end = res3.end(); it != end; it++)
    {
        std::cout << (*it) << "\n";
    }

    //rule1
    std::set< boost::dynamic_bitset<> > res4 = power->getMaxIndependent(l);
    std::cout << "\nTermos maximos independentes de 123 e 24\n";
    for(auto it = res4.begin(), end = res4.end(); it != end; it++)
    {
        std::cout << (*it) << "\n";
    }

    //S = {{1,2},{2,3,4},{1,3,4}}
    boost::dynamic_bitset<> _12(4);
    _12.set(0);
    _12.set(1);
    boost::dynamic_bitset<> _234(4);
    _234.set(1);
    _234.set(2);
    _234.set(3);
    boost::dynamic_bitset<> _134(4);
    _134.set(0);
    _134.set(2);
    _134.set(3);

    std::set< boost::dynamic_bitset<> > l2;
    l2.insert(_12);
    l2.insert(_234);
    l2.insert(_134);

    //rule2
    std::set< boost::dynamic_bitset<> > res5 = power->getMaxIndependent(l2);
    std::set< boost::dynamic_bitset<> > res6 = power->getAllParents(l2);
    std::set< boost::dynamic_bitset<> > res7 = power->filterContained(res6, res6, true);
    std::set< boost::dynamic_bitset<> > res8 = power->filterContained(res7, res5);
    std::cout << "\nCandidatos a replacement de 12 234 134\n";
    for(auto it = res8.begin(), end = res8.end(); it != end; it++)
    {
        std::cout << (*it) << "\n";
    }

    BooleanFunction::Function * g2 = new BooleanFunction::Function("g2");

    g2->addVariableToTerm(1, "c1");
    g2->addVariableToTerm(1, "c2");
    g2->addVariableToTerm(2, "c2");
    g2->addVariableToTerm(2, "c3");
    g2->addVariableToTerm(2, "c4");
    g2->addVariableToTerm(3, "c1");
    g2->addVariableToTerm(3, "c3");
    g2->addVariableToTerm(3, "c4");

    std::vector<BooleanFunction::Function*> parentsG2 = g2->getParents();
    std::cout << "\nParents of ((c1 && c2) || (c2 && c3 && c4) || (c1 && c3 && c4)) :\n";
    for(auto it = parentsG2.begin(), end = parentsG2.end(); it != end; it++)
    {
        std::string r = BooleanFunction::PrintFunction((*it));
        std::cout << "\n" << r << "\n";
    }

    BooleanFunction::Function * g1 = new BooleanFunction::Function("g1");

    g1->addVariableToTerm(1, "c1");
    g1->addVariableToTerm(1, "c2");
    g1->addVariableToTerm(1, "c3");
    g1->addVariableToTerm(2, "c2");
    g1->addVariableToTerm(2, "c4");

    std::vector<BooleanFunction::Function*> parentsG1 = g1->getParents();
    std::cout << "\nParents of ((c1 && c2 && c3) || (c2 && c4)) :\n";
    for(auto it = parentsG1.begin(), end = parentsG1.end(); it != end; it++)
    {
        std::string r = BooleanFunction::PrintFunction((*it));
        std::cout << "\n" << r << "\n";
    }

    BooleanFunction::Function * g3 = new BooleanFunction::Function("g3");

    g3->addVariableToTerm(1, "c1");
    g3->addVariableToTerm(1, "c2");
    g3->addVariableToTerm(1, "c3");
    g3->addVariableToTerm(2, "c4");

    std::vector<BooleanFunction::Function*> parentsG3 = g3->getParents();
    std::cout << "\nParents of " << BooleanFunction::PrintFunction(g3) << " :\n";
    for(auto it = parentsG3.begin(), end = parentsG3.end(); it != end; it++)
    {
        std::string r = BooleanFunction::PrintFunction((*it));
        std::cout << "\n" << r << "\n";
    }

    std::string s = "{{1,2},{3,4}}";
    BooleanFunction::Function * s1 = BooleanFunction::ParseFunction(s);
    std::cout << "\n" << BooleanFunction::PrintFunction(s1) << "\n";


    std::string ss = "(( 1 && 2 ) || ( c3 && c4)) ";
    BooleanFunction::Function * s2 = BooleanFunction::ParseFunction(ss);
    std::cout << "\n" << BooleanFunction::PrintFunction(s2) << "\n";


    if(argc > 1)
    {
        int idf = 1;
        if(argc > 2)
        {
            idf = 2;
        }
        BooleanFunction::Function * ff = BooleanFunction::ParseFunction(argv[idf]);
        if(idf == 1 || (idf == 2 && std::strcmp(argv[1],"p") == 0))
        {
            std::cout << "\nCalculating parents of " << argv[idf] << "\n";
            std::vector<BooleanFunction::Function*> parentsff = ff->getParents();
            for(auto it = parentsff.begin(), end = parentsff.end(); it != end; it++)
            {
                std::string r = BooleanFunction::PrintFunction((*it));
                std::cout << "\n" << r << "\n";
            }
        }
        if(idf == 1 || (idf == 2 && std::strcmp(argv[1],"c") == 0))
        {
            std::cout << "\nCalculating children of " << argv[idf] << "\n";
            std::vector<BooleanFunction::Function*> childrenff = ff->getChildren();
            for(auto it = childrenff.begin(), end = childrenff.end(); it != end; it++)
            {
                std::string r = BooleanFunction::PrintFunction((*it));
                std::cout << "\n" << r << "\n";
            }
        }

    }

}