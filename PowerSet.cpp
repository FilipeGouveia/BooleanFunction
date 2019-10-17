#include "PowerSet.h"

#include <math.h>

namespace BooleanFunction
{

    PowerSet::PowerSet(int d) :
        powerset_base_(),
        dimension_(d)
    {
        /* std::set< boost::dynamic_bitset<> > powerset_base_(); */
        int total = pow(2, d);
        for(int i = 1; i <= total ; i++)
        {
            boost::dynamic_bitset<> bs (d,i);
            powerset_base_.insert(bs);
        }
    }

    PowerSet::~PowerSet() {}

    int PowerSet::getDimension()
    {
        return dimension_;
    }

    std::set< boost::dynamic_bitset<> > PowerSet::getIndependent(boost::dynamic_bitset<> term)
    {
        std::set< boost::dynamic_bitset<> > result;
        for(auto it = powerset_base_.begin(), end= powerset_base_.end(); it != end; it++)
        {
            boost::dynamic_bitset<> bs = ((*it) & term);
            if(term.count() < (*it).count())
            {
                if(term != bs)
                    result.insert((*it));
            }
            else
            {
                if((*it) != bs)
                    result.insert((*it));
            }
        }

        return result;
    }

    std::set< boost::dynamic_bitset<> > PowerSet::getIndependent(std::set< boost::dynamic_bitset<> > terms)
    {
        std::set< boost::dynamic_bitset<> > result;
        for(auto it = powerset_base_.begin(), end = powerset_base_.end(); it != end; it++)
        {
            bool candidate = true;
            for(auto it2 = terms.begin(), end2= terms.end(); it2!=end2; it2++)
            {
                //can be done with is_subset_of
                boost::dynamic_bitset<> bs = ((*it) & (*it2));
                if((*it2).count() < (*it).count())
                {
                    if((*it2) == bs)
                    {
                        candidate = false;
                        break;
                    }
                }
                else
                {
                    if((*it) == bs)
                    {
                        candidate = false;
                        break;
                    }
                }
            }
            if(candidate)
                result.insert((*it));
        }

        return result;
    }

    std::set< boost::dynamic_bitset<> > PowerSet::getMaxIndependent(std::set< boost::dynamic_bitset<> > terms)
    {
        std::set< boost::dynamic_bitset<> > independents = getIndependent(terms);

        size_t max = 0;
        std::set< boost::dynamic_bitset<> > result;
        for(auto it = independents.begin(), end = independents.end(); it != end; it++)
        {
            size_t count = (*it).count();
            if(count > max)
            {
                max = count;
                result.clear();
                result.insert((*it));
            }
            else
            {
                if(count == max)
                {
                    result.insert((*it));
                }
            }
        }
        return result;
    }

    bool PowerSet::isContained(boost::dynamic_bitset<> term, std::set< boost::dynamic_bitset<> > setToTest, bool strictly)
    {
        for(auto it = setToTest.begin(), end = setToTest.end(); it != end; it++)
        {
            if(strictly)
            {
                if(term.is_proper_subset_of((*it)))
                    return true;
            }
            else
            {
                if(term.is_subset_of((*it)))
                    return true;
            }
        }
        return false;
    }

    std::set< boost::dynamic_bitset<> > PowerSet::filterContained(std::set< boost::dynamic_bitset<> > setToFilter, std::set< boost::dynamic_bitset<> > setToTest, bool strictly)
    {
        std::set< boost::dynamic_bitset<> > result;
        for(auto it = setToFilter.begin(), end = setToFilter.end(); it != end; it++)
        {
            if(!isContained((*it), setToTest, strictly))
                result.insert((*it));
        }
        return result;
    }

    std::set< boost::dynamic_bitset<> > PowerSet::getParents(boost::dynamic_bitset<> term)
    {
        std::set< boost::dynamic_bitset<> > result;
        for (int i = 0; i < dimension_; i++)
        {
            if(term.test(i))
            {
                boost::dynamic_bitset<> father = term;
                father.reset(i);
                result.insert(father);
            }
		}
		return result;
    }

    std::set< boost::dynamic_bitset<> > PowerSet::getChildren(boost::dynamic_bitset<> term)
    {
        std::set< boost::dynamic_bitset<> > result;
        for (int i = 0; i < dimension_; i++)
        {
            if(!term.test(i))
            {
                boost::dynamic_bitset<> child = term;
                child.set(i);
                result.insert(child);
            }
		}
		return result;
    }

    std::set< boost::dynamic_bitset<> > PowerSet::getAllParents(std::set< boost::dynamic_bitset<> > terms)
    {
        std::set< boost::dynamic_bitset<> > result;

        for(auto it = terms.begin(), end = terms.end(); it != end; it++)
        {
            //TODO
            std::set< boost::dynamic_bitset<> > parents = getParents((*it));
            for(auto it2 = parents.begin(), end2 = parents.end(); it2 != end2; it2++)
            {
                result.insert((*it2));
            }
        }

        return result;

    }
    std::set< boost::dynamic_bitset<> > PowerSet::getAllChildren(std::set< boost::dynamic_bitset<> > terms)
    {
        std::set< boost::dynamic_bitset<> > result;

        for(auto it = terms.begin(), end = terms.end(); it != end; it++)
        {
            //TODO
            std::set< boost::dynamic_bitset<> > children = getChildren((*it));
            for(auto it2 = children.begin(), end2 = children.end(); it2 != end2; it2++)
            {
                result.insert((*it2));
            }
        }

        return result;

    }


    bool PowerSet::isDominated(boost::dynamic_bitset<> term, std::set< boost::dynamic_bitset<> > setToTest, bool strictly)
    {
        for(auto it = setToTest.begin(), end = setToTest.end(); it != end; it++)
        {
            if(strictly)
            {
                if((*it).is_proper_subset_of(term))
                    return true;
            }
            else
            {
                if((*it).is_subset_of(term))
                    return true;
            }
        }
        return false;

    }

    std::set< boost::dynamic_bitset<> > PowerSet::filterDominated(std::set< boost::dynamic_bitset<> > setToFilter, std::set< boost::dynamic_bitset<> > setToTest, bool strictly)
    {
        std::set< boost::dynamic_bitset<> > result;
        for(auto it = setToFilter.begin(), end = setToFilter.end(); it != end; it++)
        {
            if(!isDominated((*it), setToTest, strictly))
                result.insert((*it));
        }

        return result;
    }

    bool PowerSet::isDegenerated(std::set< boost::dynamic_bitset<> > terms)
    {
        boost::dynamic_bitset<> bs (dimension_,0);
        for(auto it = terms.begin(), end = terms.end(); it != end; it++)
        {
            bs |= (*it);
            if(bs.count() == dimension_)
            {
                return false;
            }
        }
        return true;
    }

}