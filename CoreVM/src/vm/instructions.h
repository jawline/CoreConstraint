#ifndef _INSTRUCTION_DEF_H_
#define _INSTRUCTION_DEF_H_

namespace VM {
	enum {

		/**
		 * NoOp. Its traditional
		 */
		NoOp = 0x0,

		/**
		 * Load a 32 bit value into a register
		 */
		LoadImmediate,

		/**
		 * Immediate operations take a register and apply a 32bit signed integer to them
		 */
		AddImmediate,
		SubtractImmediate,
		MultiplyImmediate,
		DivideImmediate,
		GreaterThanImmediate,
		LessThanImmediate,

		/**
		 * Register operations apply one register to another, treating each as a 32 bit signed
		 * int
		 */
		AddRegister,
		SubtractRegister,
		MultiplyRegister,
		DivideRegister,
		GreaterThanRegister,
		LessThanRegister,

		/**
		 * Memory operations
		 */
		SetMemoryInt,
		GetMemoryInt,

		/**
		 * Unconditional Jumps
		 */
		JumpImmediate,
		JumpRegister,
		
		/**
		 * Branching operations
		 */
		JumpEqualImmediate,
		JumpNotEqualImmediate,
		JumpEqualRegister,
		JumpNotEqualRegister,

		NumInstructions
	};
}

#endif //_INSTRUCTION_DEF_H_
