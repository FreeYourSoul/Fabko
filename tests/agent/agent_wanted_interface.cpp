// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Copyright (c) 2022-2025.
// Created by fys on 17.03.24.
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.

#include <array>
#include <deque>
#include <optional>

#include "agent.hh"

class personality_investigator {
    //! Myers-Briggs personality types
    //! https://www.16personalities.com/personality-types
    enum class type {
        analyst_logician,     // strategist careful
        analyst_commander,    // strategist bold
        diplomat_mediator,    // altruist, careful
        diplomat_protagonist, // leader/personal interest bold
        sentinel_logistic,    // practical / cold minded, careful
        explorer_virtuoso,    // skilled, bold
    };

    // ...
};

// an agent has
// * personality {
//   + inherited needs (required for living : eat / drink / sleep)
//   + personal needs [weighting options] {
//      - 7 capital : greed / lust / pride / sloth / gluttony / envy
//      - theological virtues : faith / hope / charity
//   }
//   + ambition
// }
//
// * skills {
//    + stats {
//      - intelligence
//      - manual
//      - strength
//    }
//    + inherited skills (from parent/ from birth luck (talent))
//    + acquired skills (from experiences)
// }

/**
 * @brief wrap a value from 0 to 100 to get the percentage of completion of a task
 */
struct achieved {
    int percentage;
};

/**
 * @brief represent a task that an agent can do
 */
class task {
  private:
    //! consumption % of the capability of the agent (represent the mental burden of the task)
    int _consumption;

    //! two main task cannot be done in parallel
    //! @example for instance two tasks task that require your physical presence cannot be done at the same time
    bool _main_task;
};

struct decision_weight {
    float weight{0.0};
    int importance{0};
};

enum sins : std::uint8_t {
    ds_greed = 0, //
    ds_lust,      //
    ds_pride,     // oppose selflessness (kindness)
    ds_sloth,     // oppose active / entrepreneurship
    ds_gluttony,  // oppose restraint
    ds_envy,      //
    ds_wrath,     // oppose calm
    doubtful,     // opposite of faith
    ds_total
};

using relation_id = std::size_t;

class relationship {
  public:
    struct relation {
        std::uint8_t value;
        std::array<std::uint8_t, sins::ds_total> _sin_weight;
    };

  private:
    std::map<relation_id, relation> belief_value;
};

class skillset;
class assets;

class behavioural_unit {
    using ongoing_task = std::pair<task, achieved>;

  public:
    bool compute(const skillset& owned_skills, const assets& owned_assets);

  private:
    //    needs<vital::> _vitals;
    std::array<decision_weight, sins::ds_total> _sins;

    //! behavioural relationships. Used in order to determine the
    relationship _relationship;

    ongoing_task _current_main;
    std::vector<ongoing_task> _secondary_tasks;
};

/**
 *
 */
class asset {
    static constexpr std::int32_t SkillResult = 0;

    struct value {
        std::uint32_t actual_value;

        //! value that the agent perceive of the asset
        //! @example
        //! - A family heirloom has value only for a member of the family
        //! - A holy artifact has value only for a member of the religion it is for
        std::uint32_t believed_value;
    };

    //! id of the asset to retrieve its information from the local knowledge base
    std::int32_t asset_id{SkillResult};

    //! type of asset (defined by the logic of the implementation)
    std::int64_t type;
    //! value attributed to this asset (depends on the type of Asset it is)
    value value;
};

class assets {
    std::deque<std::pair<asset, std::int32_t>> _physical_assets;
};

class skill {

  private:
    //! results that can be acquired from the completion of the skill
    std::vector<asset> result;
};
class skillset {
    std::vector<skill> _skills;
};

class testing_agent {

  public:
    // logic implementation
    std::vector<fabko::acl::message> operator()(std::optional<fabko::acl::message> msg) {
    }

  private:
    void recompute_goals_priority();

  private:
    behavioural_unit _behaviour;
    skillset _skillset;
    assets _assets;
};

int _() {

    fabko::agent ag(testing_agent{});
}