/*
 *
 * Copyright (c) 2002
 * Dr John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         perl_matcher_common.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: Definitions of perl_matcher member functions that are 
  *                specific to the non-recursive implementation.
  */

#ifndef BOOST_REGEX_V4_PERL_MATCHER_NON_RECURSIVE_HPP
#define BOOST_REGEX_V4_PERL_MATCHER_NON_RECURSIVE_HPP

#include <new>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost{
namespace re_detail{

template <class T>
inline void inplace_destroy(T* p)
{
   (void)p;  // warning suppression
   p->~T();
}

struct saved_state
{
   unsigned int id;
   saved_state(unsigned i) : id(i) {}
};

template <class BidiIterator>
struct saved_matched_paren : public saved_state
{
   int index;
   sub_match<BidiIterator> sub;
   saved_matched_paren(int i, const sub_match<BidiIterator>& s) : saved_state(1), index(i), sub(s){};
};

template <class BidiIterator>
struct saved_position : public saved_state
{
   const re_syntax_base* pstate;
   BidiIterator position;
   saved_position(const re_syntax_base* ps, BidiIterator pos, int i) : saved_state(i), pstate(ps), position(pos){};
};

template <class BidiIterator>
struct saved_assertion : public saved_position<BidiIterator>
{
   bool positive;
   saved_assertion(bool p, const re_syntax_base* ps, BidiIterator pos) 
      : saved_position<BidiIterator>(ps, pos, saved_type_assertion), positive(p){};
};

template <class BidiIterator>
struct saved_repeater : public saved_state
{
   repeater_count<BidiIterator> count;
   saved_repeater(int i, repeater_count<BidiIterator>** s, BidiIterator start) 
      : saved_state(saved_state_repeater_count), count(i,s,start){}
};

struct saved_extra_block : public saved_state
{
   saved_state *base, *end;
   saved_extra_block(saved_state* b, saved_state* e) 
      : saved_state(saved_state_extra_block), base(b), end(e) {}
};

struct save_state_init
{
   saved_state** stack;
   save_state_init(saved_state** base, saved_state** end)
      : stack(base)
   {
      *base = static_cast<saved_state*>(get_mem_block());
      *end = reinterpret_cast<saved_state*>(reinterpret_cast<char*>(*base)+BOOST_REGEX_BLOCKSIZE);
      --(*end);
      (void) new (*end)saved_state(0);
      assert(*end > *base);
   }
   ~save_state_init()
   {
      put_mem_block(*stack);
      *stack = 0;
   }
};

template <class BidiIterator>
struct saved_single_repeat : public saved_state
{
   unsigned count;
   const re_repeat* rep;
   BidiIterator last_position;
   saved_single_repeat(unsigned c, const re_repeat* r, BidiIterator lp, int arg_id) 
      : saved_state(arg_id), count(c), rep(r), last_position(lp){}
};

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_all_states()
{
   static matcher_proc_type const s_match_vtable[26] = 
   {
      (&perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_startmark),
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_endmark,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_literal,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_start_line,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_end_line,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_wild,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_match,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_word_boundary,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_within_word,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_word_start,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_word_end,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_buffer_start,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_buffer_end,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_backref,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_long_set,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_set,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_jump,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_alt,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_rep,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_combining,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_soft_buffer_end,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_restart_continue,
      (::boost::is_random_access_iterator<BidiIterator>::value ? &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_dot_repeat_fast : &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_dot_repeat_slow),
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_char_repeat,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_set_repeat,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_long_set_repeat,
   };

   push_recursion_stopper();
   do{
      while(pstate)
      {
         matcher_proc_type proc = s_match_vtable[pstate->type];
         ++state_count;
         if(!(this->*proc)())
         {
            if(state_count > max_state_count)
               raise_error(traits_inst, REG_ESPACE);
            if((m_match_flags & match_partial) && (position == last))
               m_has_partial_match = true;
            if(false == unwind(false))
               return m_recursive_result;
         }
      }
   }while(unwind(true));
   return m_recursive_result;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::extend_stack()
{
   if(used_block_count)
   {
      --used_block_count;
      saved_state* stack_base;
      saved_state* backup_state;
      stack_base = static_cast<saved_state*>(get_mem_block());
      backup_state = reinterpret_cast<saved_state*>(reinterpret_cast<char*>(stack_base)+BOOST_REGEX_BLOCKSIZE);
      saved_extra_block* block = static_cast<saved_extra_block*>(backup_state);
      --block;
      (void) new (block) saved_extra_block(m_stack_base, m_backup_state);
      m_stack_base = stack_base;
      m_backup_state = block;
   }
   else
      raise_error(traits_inst, REG_E_MEMORY);
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
inline void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::push_matched_paren(int index, const sub_match<BidiIterator>& sub)
{
   assert(index);
   saved_matched_paren<BidiIterator>* pmp = static_cast<saved_matched_paren<BidiIterator>*>(m_backup_state);
   --pmp;
   if(pmp < m_stack_base)
   {
      extend_stack();
      pmp = static_cast<saved_matched_paren<BidiIterator>*>(m_backup_state);
      --pmp;
   }
   (void) new (pmp)saved_matched_paren<BidiIterator>(index, sub);
   m_backup_state = pmp;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
inline void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::push_recursion_stopper()
{
   saved_state* pmp = m_backup_state;
   --pmp;
   if(pmp < m_stack_base)
   {
      extend_stack();
      pmp = m_backup_state;
      --pmp;
   }
   (void) new (pmp)saved_state(saved_type_recurse);
   m_backup_state = pmp;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
inline void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::push_assertion(const re_syntax_base* ps, bool positive)
{
   saved_assertion<BidiIterator>* pmp = static_cast<saved_assertion<BidiIterator>*>(m_backup_state);
   --pmp;
   if(pmp < m_stack_base)
   {
      extend_stack();
      pmp = static_cast<saved_assertion<BidiIterator>*>(m_backup_state);
      --pmp;
   }
   (void) new (pmp)saved_assertion<BidiIterator>(positive, ps, position);
   m_backup_state = pmp;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
inline void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::push_alt(const re_syntax_base* ps)
{
   saved_position<BidiIterator>* pmp = static_cast<saved_position<BidiIterator>*>(m_backup_state);
   --pmp;
   if(pmp < m_stack_base)
   {
      extend_stack();
      pmp = static_cast<saved_position<BidiIterator>*>(m_backup_state);
      --pmp;
   }
   (void) new (pmp)saved_position<BidiIterator>(ps, position, saved_state_alt);
   m_backup_state = pmp;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
inline void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::push_non_greedy_repeat(const re_syntax_base* ps)
{
   saved_position<BidiIterator>* pmp = static_cast<saved_position<BidiIterator>*>(m_backup_state);
   --pmp;
   if(pmp < m_stack_base)
   {
      extend_stack();
      pmp = static_cast<saved_position<BidiIterator>*>(m_backup_state);
      --pmp;
   }
   (void) new (pmp)saved_position<BidiIterator>(ps, position, saved_state_non_greedy_long_repeat);
   m_backup_state = pmp;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
inline void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::push_repeater_count(int i, repeater_count<BidiIterator>** s)
{
   saved_repeater<BidiIterator>* pmp = static_cast<saved_repeater<BidiIterator>*>(m_backup_state);
   --pmp;
   if(pmp < m_stack_base)
   {
      extend_stack();
      pmp = static_cast<saved_repeater<BidiIterator>*>(m_backup_state);
      --pmp;
   }
   (void) new (pmp)saved_repeater<BidiIterator>(i, s, position);
   m_backup_state = pmp;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
inline void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::push_single_repeat(unsigned c, const re_repeat* r, BidiIterator last_position, int id)
{
   saved_single_repeat<BidiIterator>* pmp = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);
   --pmp;
   if(pmp < m_stack_base)
   {
      extend_stack();
      pmp = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);
      --pmp;
   }
   (void) new (pmp)saved_single_repeat<BidiIterator>(c, r, last_position, id);
   m_backup_state = pmp;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_startmark()
{
   int index = static_cast<const re_brace*>(pstate)->index;
   switch(index)
   {
   case 0:
      pstate = pstate->next.p;
      break;
   case -1:
   case -2:
      {
         // forward lookahead assert:
         const re_syntax_base* next_pstate = static_cast<const re_jump*>(pstate->next.p)->alt.p->next.p;
         pstate = pstate->next.p->next.p;
         push_assertion(next_pstate, index == -1);
         break;
      }
   case -3:
      {
         // independent sub-expression, currently this is always recursive:
         const re_syntax_base* next_pstate = static_cast<const re_jump*>(pstate->next.p)->alt.p->next.p;
         pstate = pstate->next.p->next.p;
         bool r = match_all_states();
         pstate = next_pstate;
#ifdef BOOST_REGEX_MATCH_EXTRA
         if(r && (m_match_flags & match_extra))
         {
            //
            // our captures have been stored in *m_presult
            // we need to unpack them, and insert them
            // back in the right order when we unwind the stack:
            //
            match_results<BidiIterator, Allocator> temp_match(*m_presult);
            unsigned i;
            for(i = 0; i < temp_match.size(); ++i)
               (*m_presult)[i].get_captures().clear();
            // match everything else:
            r = match_all_states();
            // now place the stored captures back:
            for(i = 0; i < temp_match.size(); ++i)
            {
               typedef typename sub_match<BidiIterator>::capture_sequence_type seq;
               seq& s1 = (*m_presult)[i].get_captures();
               const seq& s2 = temp_match[i].captures();
               s1.insert(
                  s1.end(), 
                  s2.begin(), 
                  s2.end());
            }
         }
#endif
         return r;
      }
   default:
   {
      assert(index > 0);
      if((m_match_flags & match_nosubs) == 0)
      {
         push_matched_paren(index, (*m_presult)[index]);
         m_presult->set_first(position, index);
      }
      pstate = pstate->next.p;
      break;
   }
   }
   return true;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_alt()
{
   bool take_first, take_second;
   const re_jump* jmp = static_cast<const re_jump*>(pstate);

   // find out which of these two alternatives we need to take:
   if(position == last)
   {
      take_first = jmp->can_be_null & mask_take;
      take_second = jmp->can_be_null & mask_skip;
   }
   else
   {
      take_first = access::can_start(*position, jmp->_map, (unsigned char)mask_take);
      take_second = access::can_start(*position, jmp->_map, (unsigned char)mask_skip);
  }

   if(take_first)
   {
      // we can take the first alternative,
      // see if we need to push next alternative:
      if(take_second)
      {
         push_alt(jmp->alt.p);
      }
      pstate = pstate->next.p;
      return true;
   }
   if(take_second)
   {
      pstate = jmp->alt.p;
      return true;
   }
   return false;  // neither option is possible
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_rep()
{
#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4127 4244)
#endif
#ifdef __BORLANDC__
#pragma option push -w-8008 -w-8066 -w-8004
#endif
   const re_repeat* rep = static_cast<const re_repeat*>(pstate);

   // find out which of these two alternatives we need to take:
   bool take_first, take_second;
   if(position == last)
   {
      take_first = rep->can_be_null & mask_take;
      take_second = rep->can_be_null & mask_skip;
   }
   else
   {
      take_first = access::can_start(*position, rep->_map, (unsigned char)mask_take);
      take_second = access::can_start(*position, rep->_map, (unsigned char)mask_skip);
   }

   if(take_first || (next_count->get_id() != rep->id))
   {
      // we're moving to a different repeat from the last
      // one, so set up a counter object:
      push_repeater_count(rep->id, &next_count);
   }
   //
   // If we've had at least one repeat already, and the last one 
   // matched the NULL string then set the repeat count to
   // maximum:
   //
   next_count->check_null_repeat(position, rep->max);

   if(next_count->get_count() < rep->min)
   {
      // we must take the repeat:
      if(take_first)
      {
         // increase the counter:
         ++(*next_count);
         pstate = rep->next.p;
         return true;
      }
      return false;
   }

   if(rep->greedy)
   {
      // try and take the repeat if we can:
      if((next_count->get_count() < rep->max) && take_first)
      {
         if(take_second)
         {
            // store position in case we fail:
            push_alt(rep->alt.p);
         }
         // increase the counter:
         ++(*next_count);
         pstate = rep->next.p;
         return true;
      }
      else if(take_second)
      {
         pstate = rep->alt.p;
         return true;
      }
      return false; // can't take anything, fail...
   }
   else // non-greedy
   {
      // try and skip the repeat if we can:
      if(take_second)
      {
         // store position in case we fail:
         push_non_greedy_repeat(rep->next.p);
         pstate = rep->alt.p;
         return true;
      }
      if((next_count->get_count() < rep->max) && take_first)
      {
         // increase the counter:
         ++(*next_count);
         pstate = rep->next.p;
         return true;
      }
   }
   return false;
#ifdef __BORLANDC__
#pragma option pop
#endif
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_dot_repeat_slow()
{
   unsigned count = 0;
   const re_repeat* rep = static_cast<const re_repeat*>(pstate);
   re_syntax_base* psingle = rep->next.p;
   // match compulsary repeats first:
   while(count < rep->min)
   {
      pstate = psingle;
      if(!match_wild())
         return false;
      ++count;
   }
   if(rep->greedy)
   {
      // repeat for as long as we can:
      while(count < rep->max)
      {
         pstate = psingle;
         if(!match_wild())
            break;
         ++count;
      }
      // remember where we got to if this is a leading repeat:
      if((rep->leading) && (count < rep->max))
         restart = position;
      // push backtrack info if available:
      if(count - rep->min)
         push_single_repeat(count, rep, position, saved_state_greedy_single_repeat);
      // jump to next state:
      pstate = rep->alt.p;
      return true;
   }
   else
   {
      // non-greedy, push state and return true if we can skip:
      if(count < rep->max)
         push_single_repeat(count, rep, position, saved_state_rep_slow_dot);
      pstate = rep->alt.p;
      return (position == last) ? (rep->can_be_null & mask_skip) : access::can_start(*position, rep->_map, mask_skip);
   }
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_dot_repeat_fast()
{
   if(m_match_flags & (match_not_dot_newline | match_not_dot_null))
      return match_dot_repeat_slow();

   const re_repeat* rep = static_cast<const re_repeat*>(pstate);
   unsigned count = (std::min)(static_cast<unsigned>(re_detail::distance(position, last)), static_cast<unsigned>(rep->greedy ? rep->max : rep->min));
   if(rep->min > count)
      return false;  // not enough text left to match
   std::advance(position, count);

   if(rep->greedy)
   {
      if((rep->leading) && (count < rep->max))
         restart = position;
      // push backtrack info if available:
      if(count - rep->min)
         push_single_repeat(count, rep, position, saved_state_greedy_single_repeat);
      // jump to next state:
      pstate = rep->alt.p;
      return true;
   }
   else
   {
      // non-greedy, push state and return true if we can skip:
      if(count < rep->max)
         push_single_repeat(count, rep, position, saved_state_rep_fast_dot);
      pstate = rep->alt.p;
      return (position == last) ? (rep->can_be_null & mask_skip) : access::can_start(*position, rep->_map, mask_skip);
   }
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_char_repeat()
{
#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4127)
#endif
#ifdef __BORLANDC__
#pragma option push -w-8008 -w-8066 -w-8004
#endif
   const re_repeat* rep = static_cast<const re_repeat*>(pstate);
   assert(1 == static_cast<const re_literal*>(rep->next.p)->length);
   const char_type what = *reinterpret_cast<const char_type*>(static_cast<const re_literal*>(rep->next.p) + 1);
   unsigned count = 0;
   //
   // start by working out how much we can skip:
   //
   unsigned desired = rep->greedy ? rep->max : rep->min;
   if(::boost::is_random_access_iterator<BidiIterator>::value)
   {
      BidiIterator end = position;
      std::advance(end, (std::min)((unsigned)re_detail::distance(position, last), desired));
      BidiIterator origin(position);
      while((position != end) && (traits_inst.translate(*position, icase) == what))
      {
         ++position;
      }
      count = (unsigned)re_detail::distance(origin, position);
   }
   else
   {
      while((count < desired) && (position != last) && (traits_inst.translate(*position, icase) == what))
      {
         ++position;
         ++count;
      }
   }

   if(count < rep->min)
      return false;

   if(rep->greedy)
   {
      if((rep->leading) && (count < rep->max))
         restart = position;
      // push backtrack info if available:
      if(count - rep->min)
         push_single_repeat(count, rep, position, saved_state_greedy_single_repeat);
      // jump to next state:
      pstate = rep->alt.p;
      return true;
   }
   else
   {
      // non-greedy, push state and return true if we can skip:
      if(count < rep->max)
         push_single_repeat(count, rep, position, saved_state_rep_char);
      pstate = rep->alt.p;
      return (position == last) ? (rep->can_be_null & mask_skip) : access::can_start(*position, rep->_map, mask_skip);
   }
#ifdef __BORLANDC__
#pragma option pop
#endif
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_set_repeat()
{
#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4127)
#endif
#ifdef __BORLANDC__
#pragma option push -w-8008 -w-8066 -w-8004
#endif
   const re_repeat* rep = static_cast<const re_repeat*>(pstate);
   const unsigned char* map = static_cast<const re_set*>(rep->next.p)->_map;
   unsigned count = 0;
   //
   // start by working out how much we can skip:
   //
   unsigned desired = rep->greedy ? rep->max : rep->min;
   if(::boost::is_random_access_iterator<BidiIterator>::value)
   {
      BidiIterator end = position;
      std::advance(end, (std::min)((unsigned)re_detail::distance(position, last), desired));
      BidiIterator origin(position);
      while((position != end) && map[(traits_uchar_type)traits_inst.translate(*position, icase)])
      {
         ++position;
      }
      count = (unsigned)re_detail::distance(origin, position);
   }
   else
   {
      while((count < desired) && (position != last) && map[(traits_uchar_type)traits_inst.translate(*position, icase)])
      {
         ++position;
         ++count;
      }
   }

   if(count < rep->min)
      return false;

   if(rep->greedy)
   {
      if((rep->leading) && (count < rep->max))
         restart = position;
      // push backtrack info if available:
      if(count - rep->min)
         push_single_repeat(count, rep, position, saved_state_greedy_single_repeat);
      // jump to next state:
      pstate = rep->alt.p;
      return true;
   }
   else
   {
      // non-greedy, push state and return true if we can skip:
      if(count < rep->max)
         push_single_repeat(count, rep, position, saved_state_rep_short_set);
      pstate = rep->alt.p;
      return (position == last) ? (rep->can_be_null & mask_skip) : access::can_start(*position, rep->_map, mask_skip);
   }
#ifdef __BORLANDC__
#pragma option pop
#endif
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::match_long_set_repeat()
{
#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4127)
#endif
#ifdef __BORLANDC__
#pragma option push -w-8008 -w-8066 -w-8004
#endif
   const re_repeat* rep = static_cast<const re_repeat*>(pstate);
   const re_set_long* set = static_cast<const re_set_long*>(pstate->next.p);
   unsigned count = 0;
   //
   // start by working out how much we can skip:
   //
   unsigned desired = rep->greedy ? rep->max : rep->min;
   if(::boost::is_random_access_iterator<BidiIterator>::value)
   {
      BidiIterator end = position;
      std::advance(end, (std::min)((unsigned)re_detail::distance(position, last), desired));
      BidiIterator origin(position);
      while((position != end) && (position != re_is_set_member(position, last, set, re)))
      {
         ++position;
      }
      count = (unsigned)re_detail::distance(origin, position);
   }
   else
   {
      while((count < desired) && (position != last) && (position != re_is_set_member(position, last, set, re)))
      {
         ++position;
         ++count;
      }
   }

   if(count < rep->min)
      return false;

   if(rep->greedy)
   {
      if((rep->leading) && (count < rep->max))
         restart = position;
      // push backtrack info if available:
      if(count - rep->min)
         push_single_repeat(count, rep, position, saved_state_greedy_single_repeat);
      // jump to next state:
      pstate = rep->alt.p;
      return true;
   }
   else
   {
      // non-greedy, push state and return true if we can skip:
      if(count < rep->max)
         push_single_repeat(count, rep, position, saved_state_rep_long_set);
      pstate = rep->alt.p;
      return (position == last) ? (rep->can_be_null & mask_skip) : access::can_start(*position, rep->_map, mask_skip);
   }
#ifdef __BORLANDC__
#pragma option pop
#endif
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
}

/****************************************************************************

Unwind and associated proceedures follow, these perform what normal stack
unwinding does in the recursive implementation.

****************************************************************************/

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind(bool have_match)
{
   static unwind_proc_type const s_unwind_table[14] = 
   {
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_end,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_paren,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_recursion_stopper,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_assertion,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_alt,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_repeater_counter,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_extra_block,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_greedy_single_repeat,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_slow_dot_repeat,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_fast_dot_repeat,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_char_repeat,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_short_set_repeat,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_long_set_repeat,
      &perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_non_greedy_repeat,
   };

   m_recursive_result = have_match;
   unwind_proc_type unwinder;
   bool cont;
   //
   // keep unwinding our stack until we have something to do:
   //
   do
   {
      unwinder = s_unwind_table[m_backup_state->id];
      cont = (this->*unwinder)(m_recursive_result);
   }while(cont);
   //
   // return true if we have more states to try:
   //
   return pstate ? true : false;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_end(bool)
{
   pstate = 0;   // nothing left to search
   return false; // end of stack nothing more to search
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_paren(bool have_match)
{
   saved_matched_paren<BidiIterator>* pmp = static_cast<saved_matched_paren<BidiIterator>*>(m_backup_state);
   // restore previous values if no match was found:
   if(have_match == false)
   {
      m_presult->set_first(pmp->sub.first, pmp->index);
      m_presult->set_second(pmp->sub.second, pmp->index, pmp->sub.matched);
   }
#ifdef BOOST_REGEX_MATCH_EXTRA
   //
   // we have a match, push the capture information onto the stack:
   //
   else if(pmp->sub.matched && (match_extra & m_match_flags))
      ((*m_presult)[pmp->index]).get_captures().push_back(pmp->sub);
#endif
   // unwind stack:
   m_backup_state = pmp+1;
   boost::re_detail::inplace_destroy(pmp);
   return true; // keep looking
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_recursion_stopper(bool)
{
   boost::re_detail::inplace_destroy(m_backup_state++);
   pstate = 0;   // nothing left to search
   return false; // end of stack nothing more to search
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_assertion(bool r)
{
   saved_assertion<BidiIterator>* pmp = static_cast<saved_assertion<BidiIterator>*>(m_backup_state);
   pstate = pmp->pstate;
   position = pmp->position;
   bool result = (r == pmp->positive);
   m_recursive_result = pmp->positive ? r : !r;
   boost::re_detail::inplace_destroy(pmp++);
   m_backup_state = pmp;
   return !result; // return false if the assertion was matched to stop search.
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_alt(bool r)
{
   saved_position<BidiIterator>* pmp = static_cast<saved_position<BidiIterator>*>(m_backup_state);
   if(!r)
   {
      pstate = pmp->pstate;
      position = pmp->position;
   }
   boost::re_detail::inplace_destroy(pmp++);
   m_backup_state = pmp;
   return r; 
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_repeater_counter(bool)
{
   saved_repeater<BidiIterator>* pmp = static_cast<saved_repeater<BidiIterator>*>(m_backup_state);
   boost::re_detail::inplace_destroy(pmp++);
   m_backup_state = pmp;
   return true; // keep looking
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_extra_block(bool)
{
   saved_extra_block* pmp = static_cast<saved_extra_block*>(m_backup_state);
   void* condemmed = m_stack_base;
   m_stack_base = pmp->base;
   m_backup_state = pmp->end;
   boost::re_detail::inplace_destroy(pmp);
   put_mem_block(condemmed);
   return true; // keep looking
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
inline void perl_matcher<BidiIterator, Allocator, traits, Allocator2>::destroy_single_repeat()
{
   saved_single_repeat<BidiIterator>* p = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);
   boost::re_detail::inplace_destroy(p++);
   m_backup_state = p;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_greedy_single_repeat(bool r)
{
   saved_single_repeat<BidiIterator>* pmp = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);

   // if we have a match, just discard this state:
   if(r) 
   {
      destroy_single_repeat();
      return true;
   }

   const re_repeat* rep = pmp->rep;
   unsigned count = pmp->count;
   assert(rep->next.p != 0);
   assert(rep->alt.p != 0);

   count -= rep->min;
   
   if((m_match_flags & match_partial) && (position == last))
      m_has_partial_match = true;

   assert(count);
   position = pmp->last_position;

   // backtrack till we can skip out:
   do
   {
      --position;
      --count;
      ++state_count;
   }while(count && !access::can_start(*position, rep->_map, mask_skip));

   // if we've hit base, destroy this state:
   if(count == 0)
   {
         destroy_single_repeat();
         if(!access::can_start(*position, rep->_map, mask_skip))
            return true;
   }
   else
   {
      pmp->count = count + rep->min;
      pmp->last_position = position;
   }
   pstate = rep->alt.p;
   return false;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_slow_dot_repeat(bool r)
{
   saved_single_repeat<BidiIterator>* pmp = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);

   // if we have a match, just discard this state:
   if(r) 
   {
      destroy_single_repeat();
      return true;
   }

   const re_repeat* rep = pmp->rep;
   unsigned count = pmp->count;
   assert(rep->type == syntax_element_dot_rep);
   assert(rep->next.p != 0);
   assert(rep->alt.p != 0);
   assert(rep->next.p->type == syntax_element_wild);

   assert(count < rep->max);
   pstate = rep->next.p;
   position = pmp->last_position;

   if(position != last)
   {
      // wind forward until we can skip out of the repeat:
      do
      {
         if(!match_wild())
         {
            // failed repeat match, discard this state and look for another:
            destroy_single_repeat();
            return true;
         }
         ++count;
         ++state_count;
         pstate = rep->next.p;
      }while((count < rep->max) && (position != last) && !access::can_start(*position, rep->_map, mask_skip));
   }   
   if(position == last)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(0 == (rep->can_be_null & mask_skip))
         return true;
   }
   else if(count == rep->max)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(!access::can_start(*position, rep->_map, mask_skip))
         return true;
   }
   else
   {
      pmp->count = count;
      pmp->last_position = position;
   }
   pstate = rep->alt.p;
   return false;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_fast_dot_repeat(bool r)
{
   saved_single_repeat<BidiIterator>* pmp = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);

   // if we have a match, just discard this state:
   if(r) 
   {
      destroy_single_repeat();
      return true;
   }

   const re_repeat* rep = pmp->rep;
   unsigned count = pmp->count;

   assert(count < rep->max);
   position = pmp->last_position;
   if(position != last)
   {

      // wind forward until we can skip out of the repeat:
      do
      {
         ++position;
         ++count;
         ++state_count;
      }while((count < rep->max) && (position != last) && !access::can_start(*position, rep->_map, mask_skip));
   }

   if(position == last)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(0 == (rep->can_be_null & mask_skip))
         return true;
   }
   else if(count == rep->max)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(!access::can_start(*position, rep->_map, mask_skip))
         return true;
   }
   else
   {
      pmp->count = count;
      pmp->last_position = position;
   }
   pstate = rep->alt.p;
   return false;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_char_repeat(bool r)
{
   saved_single_repeat<BidiIterator>* pmp = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);

   // if we have a match, just discard this state:
   if(r) 
   {
      destroy_single_repeat();
      return true;
   }

   const re_repeat* rep = pmp->rep;
   unsigned count = pmp->count;
   pstate = rep->next.p;
   const char_type what = *reinterpret_cast<const char_type*>(static_cast<const re_literal*>(pstate) + 1);
   position = pmp->last_position;

   assert(rep->type == syntax_element_char_rep);
   assert(rep->next.p != 0);
   assert(rep->alt.p != 0);
   assert(rep->next.p->type == syntax_element_literal);
   assert(count < rep->max);

   if(position != last)
   {
      // wind forward until we can skip out of the repeat:
      do
      {
         if(traits_inst.translate(*position, icase) != what)
         {
            // failed repeat match, discard this state and look for another:
            destroy_single_repeat();
            return true;
         }
         ++count;
         ++ position;
         ++state_count;
         pstate = rep->next.p;
      }while((count < rep->max) && (position != last) && !access::can_start(*position, rep->_map, mask_skip));
   }   
   if(position == last)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(0 == (rep->can_be_null & mask_skip))
         return true;
   }
   else if(count == rep->max)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(!access::can_start(*position, rep->_map, mask_skip))
         return true;
   }
   else
   {
      pmp->count = count;
      pmp->last_position = position;
   }
   pstate = rep->alt.p;
   return false;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_short_set_repeat(bool r)
{
   saved_single_repeat<BidiIterator>* pmp = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);

   // if we have a match, just discard this state:
   if(r) 
   {
      destroy_single_repeat();
      return true;
   }

   const re_repeat* rep = pmp->rep;
   unsigned count = pmp->count;
   pstate = rep->next.p;
   const unsigned char* map = static_cast<const re_set*>(rep->next.p)->_map;
   position = pmp->last_position;

   assert(rep->type == syntax_element_short_set_rep);
   assert(rep->next.p != 0);
   assert(rep->alt.p != 0);
   assert(rep->next.p->type == syntax_element_set);
   assert(count < rep->max);
   
   if(position != last)
   {
      // wind forward until we can skip out of the repeat:
      do
      {
         if(!map[(traits_uchar_type)traits_inst.translate(*position, icase)])
         {
            // failed repeat match, discard this state and look for another:
            destroy_single_repeat();
            return true;
         }
         ++count;
         ++ position;
         ++state_count;
         pstate = rep->next.p;
      }while((count < rep->max) && (position != last) && !access::can_start(*position, rep->_map, mask_skip));
   }   
   if(position == last)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(0 == (rep->can_be_null & mask_skip))
         return true;
   }
   else if(count == rep->max)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(!access::can_start(*position, rep->_map, mask_skip))
         return true;
   }
   else
   {
      pmp->count = count;
      pmp->last_position = position;
   }
   pstate = rep->alt.p;
   return false;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_long_set_repeat(bool r)
{
   saved_single_repeat<BidiIterator>* pmp = static_cast<saved_single_repeat<BidiIterator>*>(m_backup_state);

   // if we have a match, just discard this state:
   if(r)
   {
      destroy_single_repeat();
      return true;
   }

   const re_repeat* rep = pmp->rep;
   unsigned count = pmp->count;
   pstate = rep->next.p;
   const re_set_long* set = static_cast<const re_set_long*>(pstate);
   position = pmp->last_position;

   assert(rep->type == syntax_element_long_set_rep);
   assert(rep->next.p != 0);
   assert(rep->alt.p != 0);
   assert(rep->next.p->type == syntax_element_long_set);
   assert(position != last);
   assert(count < rep->max);

   if(position != last)
   {
      // wind forward until we can skip out of the repeat:
      do
      {
         if(position == re_is_set_member(position, last, set, re))
         {
            // failed repeat match, discard this state and look for another:
            destroy_single_repeat();
            return true;
         }
         ++position;
         ++count;
         ++state_count;
         pstate = rep->next.p;
      }while((count < rep->max) && (position != last) && !access::can_start(*position, rep->_map, mask_skip));
   }   
   if(position == last)
   {
      // can't repeat any more, remove the pushed state:
      destroy_single_repeat();
      if(0 == (rep->can_be_null & mask_skip))
         return true;
   }
   else if(count == rep->max)
   {
      // can't repeat any more, remove the pushed state: 
      destroy_single_repeat();
      if(!access::can_start(*position, rep->_map, mask_skip))
         return true;
   }
   else
   {
      pmp->count = count;
      pmp->last_position = position;
   }
   pstate = rep->alt.p;
   return false;
}

template <class BidiIterator, class Allocator, class traits, class Allocator2>
bool perl_matcher<BidiIterator, Allocator, traits, Allocator2>::unwind_non_greedy_repeat(bool r)
{
   saved_position<BidiIterator>* pmp = static_cast<saved_position<BidiIterator>*>(m_backup_state);
   if(!r)
   {
      position = pmp->position;
      pstate = pmp->pstate;
      ++(*next_count);
   }
   boost::re_detail::inplace_destroy(pmp++);
   m_backup_state = pmp;
   return r;
}

} // namespace re_detail
} // namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif


