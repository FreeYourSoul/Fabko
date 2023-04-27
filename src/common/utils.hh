//
// Created by perso on 14/03/23.
//

#ifndef FABKO_UTILS_HH
#define FABKO_UTILS_HH

namespace fabko {

//! Overload pattern for visitation
template<class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}// namespace fabko

#endif//FABKO_UTILS_HH
