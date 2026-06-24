#pragma once

#include "IAction.hpp"
#include "Position.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sw
{

	struct TurnContext;

	/// @brief The physical nature of a unit, independent of its behaviour.
	///
	/// Declarative on purpose: a new unit describes *what it is* by filling this
	/// struct (designated initializers), and *what it does* by composing actions.
	/// Core services (Map, attacks, movement) read these flags; they never switch
	/// on a concrete unit type.
	struct UnitTraits
	{
		/// HP pool. `std::nullopt` means the unit has no health concept at all
		/// (e.g. a Mine): it cannot be damaged or healed and only leaves play when
		/// it removes itself.
		std::optional<int32_t> hp{};

		/// Cells the unit may cross per turn. 0 == stationary (never moves).
		uint32_t movementSpeed = 1;

		/// Whether the unit blocks its cell. Flying units and mines do not.
		bool occupiesCell = true;

		/// Whether the unit is a valid target for melee / ranged attacks.
		bool meleeTargetable = true;
		bool rangedTargetable = true;

		/// How much an incoming ranged attack's min and max distance shrink when
		/// targeting this unit (a flying Crow is reached one cell sooner and one
		/// cell closer). 0 for ordinary units.
		uint32_t rangedDistanceReduction = 0;
	};

	/// @brief Base class for every unit. Concrete units live in Features/.
	///
	/// A unit is identity + position + traits + an ordered list of actions. Its turn
	/// is "try each action until one works". Core depends only on this class, never
	/// on a specific unit type, which is what keeps new units out of Core.
	class Unit
	{
	public:
		virtual ~Unit() = default;

		uint32_t getId() const
		{
			return _id;
		}

		const std::string& getTypeName() const
		{
			return _typeName;
		}

		Position getPosition() const
		{
			return _position;
		}

		void moveTo(Position position)
		{
			_position = position;
		}

		// --- health -----------------------------------------------------------

		bool hasHealth() const
		{
			return _hp.has_value();
		}

		/// @return current HP, or 0 if the unit has no health concept.
		int32_t getHp() const
		{
			return _hp.value_or(0);
		}

		bool isAlive() const
		{
			return _alive;
		}

		/// @brief Apply damage. No-op for units without health. Drops the unit out
		///        of play when HP reaches zero.
		void takeDamage(uint32_t amount);

		/// @brief Restore HP. No-op for units without health. (No upper cap — see
		///        KNOWN_ISSUES.)
		void heal(uint32_t amount);

		/// @brief Remove the unit from play directly, regardless of HP (a Mine uses
		///        this after it detonates).
		void kill()
		{
			_alive = false;
		}

		// --- traits (read by Core services) -----------------------------------

		uint32_t movementSpeed() const
		{
			return _traits.movementSpeed;
		}

		bool occupiesCell() const
		{
			return _traits.occupiesCell;
		}

		bool canBeMeleeAttacked() const
		{
			return _traits.meleeTargetable;
		}

		bool canBeRangedAttacked() const
		{
			return _traits.rangedTargetable;
		}

		uint32_t rangedDistanceReduction() const
		{
			return _traits.rangedDistanceReduction;
		}

		// --- march target ------------------------------------------------------

		bool hasMarchTarget() const
		{
			return _marchTarget.has_value();
		}

		std::optional<Position> getMarchTarget() const
		{
			return _marchTarget;
		}

		void setMarchTarget(Position target)
		{
			_marchTarget = target;
		}

		void clearMarchTarget()
		{
			_marchTarget.reset();
		}

		// --- turn --------------------------------------------------------------

		/// @brief Run the unit's turn: first applicable action wins.
		/// @return true if the unit did anything this turn.
		bool act(TurnContext& context);

	protected:
		Unit(uint32_t id, std::string typeName, Position position, UnitTraits traits);

		/// @brief Append an action to this unit's turn sequence (call in the ctor).
		template <typename TAction, typename... Args>
		void addAction(Args&&... args)
		{
			_actions.push_back(std::make_unique<TAction>(std::forward<Args>(args)...));
		}

	private:
		uint32_t _id;
		std::string _typeName;
		Position _position;
		UnitTraits _traits;
		bool _alive = true;
		std::optional<int32_t> _hp;
		std::optional<Position> _marchTarget;
		std::vector<std::unique_ptr<IAction>> _actions;
	};

}  // namespace sw
