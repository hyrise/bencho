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
							} \
							b.full(); \
							return 0; \
						}

#endif //MAIN_H