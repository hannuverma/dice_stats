#include <iostream>
#include <random>
#include <fstream>
#include <chrono>
#include <omp.h>

std::ofstream writeFile(const std::string& fileName, unsigned long long int NoOfPermutations, unsigned int random, double error, double elapsed);
std::ifstream readFile(const std::string& fileName);
std::ofstream deleteData(const std::string& fileName);
bool isFileEmpty(const std::string& fileName);

int password{ 10212 };

int numberOfRepetition(int number, int count)
{
    std::cout << "\nNumber of " << number << "'s appeared are " << count;
    return 0;
}

long long int chance(int number, int repeated, unsigned long long int permutations)
{
    std::cout << "\n" << number << " had " << (repeated * 100.0) / permutations << "% chance of appearing.";
    return 0;
}

double modulus(double number)
{
    return number >= 0 ? number : -number;
}

int main()
{
    while (true)
    {
        double error{};
        unsigned int seed{};
        unsigned long long int iterations{};
        bool wannaSee{};
        double mean{};

    start:
        std::cout << "\nEnter any random positive integer: ";
        std::cin >> seed;

        std::cout << "Enter the number of iterations: ";
        std::cin >> iterations;

        std::cout << "Do you want to see all the iterations (1 for yes, 0 for no): ";
        std::cin >> wannaSee;

        if (iterations == password && seed == password)
        {
            std::cout << "\nDeleting all data!";
            deleteData("data.txt");

            if (isFileEmpty("data.txt"))
                std::cout << "\nThe file was successfully deleted!";
            else
                std::cout << "\nUnable to delete the file!!";

            goto start;
        }

        long long int dieNumber[6]{ 0, 0, 0, 0, 0, 0 };

        auto start = std::chrono::high_resolution_clock::now();

#pragma omp parallel
        {
            std::mt19937 mt_local(seed + omp_get_thread_num());
            std::uniform_int_distribution<int> die(1, 6);
            long long int local_dieNumber[6]{ 0, 0, 0, 0, 0, 0 };

#pragma omp for
            for (long long int i = 0; i < iterations; ++i)
            {

                long long int value = die(mt_local);
                local_dieNumber[value - 1]++;

                if (wannaSee)
                {
#pragma omp critical
                    {
                        std::cout << value << '\t';
                        if ((i + 1) % 10 == 0)
                            std::cout << "\n";
                    }
                }
            }

#pragma omp critical
            {
                for (long long int i = 0; i < 6; ++i)
                    dieNumber[i] += local_dieNumber[i];
            }
        }

        std::cout << '\n' << "For " << iterations << " iterations:";

        long long int total{};
        for (int i = 0; i < 6; i++)
        {
            numberOfRepetition(i + 1, dieNumber[i]);
            total += dieNumber[i];
        }

        for (int i = 0; i < 6; i++)
        {
            chance(i + 1, dieNumber[i], iterations);
        }

        mean = total / 6.0;

        error = (
            (modulus(dieNumber[0] - mean) +
             modulus(dieNumber[1] - mean) +
             modulus(dieNumber[2] - mean) +
             modulus(dieNumber[3] - mean) +
             modulus(dieNumber[4] - mean) +
             modulus(dieNumber[5] - mean)) / 6.0) * 100.0 / mean;

        std::cout << "\nPercentage mean average error in probability is: " << error << "%";
        std::cout << "\nthe given die is " << 100 - error << "% as accurate as a perfect die";

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "\nTime taken to complete the given iterations: " << elapsed.count() << " seconds";

        writeFile("data.txt", iterations, seed, error, elapsed.count());
    }

    return 0;
}