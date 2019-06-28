#ifndef MYRANDOM_H
#define MYRANDOM_H
#include <random>
#include <type_traits>
#include <chrono>

/*
 * A simple random number generator class.
 * Authon: Janne Kauppinen.
 * Copyright: none.
*/
template <typename T>
class MyRandom
{
    /// Determines the distribution type.
    typedef typename std::conditional<std::is_integral<T>::value,
                                          std::uniform_int_distribution<T>,
                                          std::uniform_real_distribution<T>
                                          >::type dist_type;

    public:
        /// Default contructor. Takes the seed from the system clock.
        MyRandom();
        /// Constructor with one parameter @seed.
        MyRandom(const std::string& seed);
        /// Set the range of distribution.
        void setDistribution(T a, T b);
        /// Returns a random number from current range or from default range (0.99).
        T operator()() {return dist_(mt_);}

        //~MyRandom();

    private:
        /// The random number generator.
        std::mt19937 mt_;
        /// The distrubution range.
        dist_type dist_ = dist_type(0,99);
};

template <typename T>
MyRandom<T>::MyRandom()
{
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt_ = std::mt19937(seed);
}

template <typename T>
MyRandom<T>::MyRandom(const std::string& seed)
{
    std::seed_seq s(seed.begin(),seed.end());
    mt_ = std::mt19937(s);
}

template <typename T>
inline void MyRandom<T>::setDistribution(T a, T b)
{
    dist_ = dist_type(a,b);
}

#endif // MYRANDOM_H
