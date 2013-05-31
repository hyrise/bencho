/**
 * @file main.h
 *
 * Contains the makro for the main() function used in own benchmarks.
 */

#ifndef MAIN_H
#define MAIN_H

#define MAIN(BENCHMARK) int main(int argc, char* argv[]) \
						{ \
							BENCHMARK b; \
							for (int i = 1; i < argc; ++i) \
							{ \
								std::string argument = std::string(argv[i]); \
								if (argument == "-fast") b.setFastMode(true); \
								if (argument == "-silent") b.setSilentMode(true); \
								if (argument == "-plotonly") \
								{ \
									b.plotResults(false); \
									return 0; \
								} \
							} \
							b.full(); \
							return 0; \
						}

#endif //MAIN_H