#pragma once

namespace sw
{

	class Unit;
	struct TurnContext;

	/// @brief One thing a unit can attempt on its turn (attack, heal, move, explode...).
	///
	/// A unit owns an ordered list of actions and, on its turn, tries them in order
	/// until one succeeds. This is the single extension point for behaviour: new
	/// mechanics are new IAction implementations in Features/, and Core never needs
	/// to know what they do.
	class IAction
	{
	public:
		virtual ~IAction() = default;

		/// @brief Attempt the action.
		/// @return true if the unit actually did something (and thus spends its turn);
		///         false if the action was not applicable, so the next action is tried.
		virtual bool perform(Unit& self, TurnContext& context) = 0;
	};

}  // namespace sw
