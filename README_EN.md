# Divisor Stars - Plotting Integer Divisors with Asterisks

<!-- markdownlint-disable MD041 -->
[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/chotto2/dstar-dev)

A comprehensive list that visualizes the patterns and tendencies of integer divisors, enabling observation of their overall structure.

## Overview

This program examines and displays divisors of integers from 0 to 1024 using the Sieve of Eratosthenes, representing them with asterisks.
This is the origin program that led to the publicly released Prime Oasis.
The inspiration for creating the Prime Oasis command suite came from the patterns plotted by this program (hereafter referred to as dstar).
In object-oriented terms, it can be considered a superclass or parent class.
Therefore, without understanding the divisor patterns output by dstar, it would be difficult to comprehend the Prime Oasis README.
This serves as the fundamental foundation.
The resulting list is available in results/resultds.txt - please take a look!

## Features

- 🐳 **Docker Support** - Reproducible build environment
- 📊 **Divisors up to 1024** - Suitable size for educational and research purposes

## Requirements

- Docker Desktop
- Git

## Build and Run

Building and running are necessary if you want to customize the dstar command.
By changing the N_MAX define value, you can increase or decrease the output range.

```bash
# Clone the repository
git clone https://github.com/chotto2/dstar-dev.git
cd dstar-dev

# Build Docker image
docker build -t dstar-dev .

# Run (list output)
docker run -it dstar-dev /app/build/dstar
```

## Performance

```text
real    0m0.657s
user    0m0.021s
sys     0m0.019s
```

※Codespace: 2-Core  
※Without output redirection

## Output Example

Sample output from `dstar-dev`:

```text
      n:   d(n):divisors2(n, 1024)
      0:   1024:******************************** ...
      1:      1:*
      2:      2:**
      3:      2:* *
      4:      3:** *
      5:      2:*   *
      6:      4:***  *
      7:      2:*     * 
      8:      4:** *   *
      9:      3:* *     *
     10:      4:**  *    *
     11:      2:*         *
     12:      6:**** *     *
     13:      2:*           *
     14:      4:**    *      *
     15:      4:* * *         *
...
```

The first line indicates that each list row consists of three fields separated by ':'.  
The first field, n, represents the target integer value.  
The second field, d(n), indicates the number of divisors of integer n.  
The third field, divisor2(n, 1024), shows asterisks plotted at divisor positions.  
The positions of divisors (asterisks) are in ascending order 1, 2, 3... from closest to the second field.  
divisor2(n, 1024) finds divisors limited to an upper bound of 1024 and displays the results with asterisks.  

For example, looking at the output for integer 6:

```text
      6:      4:***  * 
```

This shows that integer 6 has 4 divisors: {1,2,3,6}. (Positions 4 and 5 are blank)

※Integer 0 is a special case where all non-zero integers are divisors (n × 0 = 0)

## Noteworthy Observations

- When the second field value is 2, n is a prime number.
- VOID regions have no divisors (such as n<divisor or n/2<divisor<n)

```text
      n:   d(n):divisors2(n, 1024)
      0:   1024:******************************** ...
      1:      1:*
      2:      2:**
      3:      2:* *
      4:      3:** *
      5:      2:*   *
      6:      4:***  *
      7:      2:*     *          (VOID)
      8:      4:** *   *
      9:      3:* *     *
     10:      4:**  *    *
     11:      2:*         *
     12:      6:**** *     *
     13:      2:*           *
     14:      4:**    *(VOID)*
     15:      4:* * *         *
...
```

- Consecutive asterisks from the beginning form prime deserts (n=12, 60, etc.).  
  Below, asterisks are changed to numbers to show the structure of prime deserts.  
  Prime deserts have an umbrella-like shape and affect divisors of neighboring integers.  
  The range of influence is (2 × number of consecutive divisors) + 1.  
  However, divisors at center point ±1 are 1, so these positions may be prime.  
  Indeed, in this example, positions ±1 are prime numbers.  

```text
     54:      8:***  6  *        *        *                          *
     55:      4:*   5     *                                           *
     56:      8:** 4  **     *             *                           * 
     57:      4:* 3               *                                     *
     58:      4:*2                          *                            *
     59:      2:1                                                         *
     60:     12:123456   * *  *    *         *                             *
     61:      2:1                                                           *
     62:      4:*2                            *                              *
     63:      6:* 3   * *           *                                         *
     64:      7:** 4   *       *               *                               *
     65:      4:*   5       *                                                   *
     66:      8:***  6    *          *          *                                *
```

- The divisor pattern of integer n unfolds in the same pattern at 45-degree angles forward and backward.
  Let's examine the case of n=6.  
  Related divisors are changed from asterisks to numbers.  
  The divisors of n=6 are {1,2,3,6}.  
  When unfolded at 45-degree angles forward and backward, it appears as shown below.  
  I don't know if there's a mathematical theorem for this, but it's a discovery from observation.  
  Let's call it the "45-degree mirror image conjecture of divisors."

```text
      n:   d(n):divisors2(n, 1024)
      0:   1024:*****6************************** ...
      1:      1:*
      2:      2:**
      3:      2:* 3
      4:      3:*2 *
      5:      2:1   *
      6:      4:123  6
      7:      2:1     *
      8:      4:*2 *   *
      9:      3:* 3     *
     10:      4:**  *    *
     11:      2:*         *
     12:      6:**** 6     *
     13:      2:*           *
     14:      4:**    *      *
     15:      4:* * *         *
...
```

## Technical Details

- **Language**: C
- **Library**: GMP (GNU Multiple Precision Arithmetic Library)
- **Build System**: CMake
- **Divisor Range**: 0..1024

## Important Notes

⚠️ **Important**: This version is an implementation for educational and research purposes. Since it handles divisors up to integer 1024, it does not affect modern cryptographic systems (such as RSA-4096).

## Future Plans

- 📝 Paper submission to arXiv planned
- 📚 Detailed theoretical background of the algorithm

## License

MIT License

## Author

N.Arai

## Citation

Paper in preparation. Proper citation format will be provided after publication.

## Related Links

- Paper: arXiv (in preparation)
- Repository: <https://github.com/chotto2/dstar-dev>
