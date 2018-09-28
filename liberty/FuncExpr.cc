// OpenSTA, Static Timing Analyzer
// Copyright (c) 2018, Parallax Software, Inc.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Machine.hh"
#include "StringUtil.hh"
#include "Liberty.hh"
#include "Network.hh"
#include "FuncExpr.hh"

namespace sta {

FuncExpr *
FuncExpr::makePort(LibertyPort *port)
{
  return new FuncExpr(op_port, NULL, NULL, port);
}

FuncExpr *
FuncExpr::makeNot(FuncExpr *expr)
{
  return new FuncExpr(op_not, expr, NULL, NULL);
}


FuncExpr *
FuncExpr::makeAnd(FuncExpr *left,
		  FuncExpr *right)
{
  return new FuncExpr(op_and, left, right, NULL);
}

FuncExpr *
FuncExpr::makeOr(FuncExpr *left,
		 FuncExpr *right)
{
  return new FuncExpr(op_or, left, right, NULL);
}

FuncExpr *
FuncExpr::makeXor(FuncExpr *left,
		  FuncExpr *right)
{
  return new FuncExpr(op_xor, left, right, NULL);
}

FuncExpr *
FuncExpr::makeZero()
{
  return new FuncExpr(op_zero, NULL, NULL, NULL);
}

FuncExpr *
FuncExpr::makeOne()
{
  return new FuncExpr(op_one, NULL, NULL, NULL);
}

FuncExpr::FuncExpr(Operator op,
		   FuncExpr *left,
		   FuncExpr *right,
		   LibertyPort *port) :
  op_(op),
  left_(left),
  right_(right),
  port_(port)
{
}

void
FuncExpr::deleteSubexprs()
{
  if (left_)
    left_->deleteSubexprs();
  if (right_)
    right_->deleteSubexprs();
  delete this;
}

LibertyPort *
FuncExpr::port() const
{
  if (op_ == op_port)
    return port_;
  else
    return NULL;
}

// Protect against null sub-expressions caused by unknown port refs.
TimingSense
FuncExpr::portTimingSense(const LibertyPort *port) const
{
  TimingSense left_sense, right_sense;

  switch (op_) {
  case op_port:
    if (port == port_)
      return timing_sense_positive_unate;
    else
      return timing_sense_none;
  case op_not:
    if (left_) {
      switch (left_->portTimingSense(port)) {
      case timing_sense_positive_unate:
	return timing_sense_negative_unate;
      case timing_sense_negative_unate:
	return timing_sense_positive_unate;
      case timing_sense_non_unate:
	return timing_sense_non_unate;
      case timing_sense_none:
	return timing_sense_none;
      case timing_sense_unknown:
	return timing_sense_unknown;
      }
    }
    return timing_sense_unknown;
  case op_or:
  case op_and:
    left_sense = timing_sense_unknown;
    right_sense = timing_sense_unknown;
    if (left_)
      left_sense = left_->portTimingSense(port);
    if (right_)
      right_sense = right_->portTimingSense(port);

    if (left_sense == right_sense)
      return left_sense;
    else if (left_sense == timing_sense_non_unate
	     || right_sense == timing_sense_non_unate
	     || (left_sense == timing_sense_positive_unate
		 && right_sense == timing_sense_negative_unate)
	     || (left_sense == timing_sense_negative_unate
		 && right_sense == timing_sense_positive_unate))
      return timing_sense_non_unate;
    else if (left_sense == timing_sense_none
	     || left_sense == timing_sense_unknown)
      return right_sense;
    else if (right_sense == timing_sense_none
	     || right_sense == timing_sense_unknown)
      return left_sense;
    else
      return timing_sense_unknown;
  case op_xor:
    left_sense = timing_sense_unknown;
    right_sense = timing_sense_unknown;
    if (left_)
      left_sense = left_->portTimingSense(port);
    if (right_)
      right_sense = right_->portTimingSense(port);
    if (left_sense == timing_sense_positive_unate
	|| left_sense == timing_sense_negative_unate
	|| left_sense == timing_sense_non_unate
	|| right_sense == timing_sense_positive_unate
	|| right_sense == timing_sense_negative_unate
	|| right_sense == timing_sense_non_unate)
      return timing_sense_non_unate;
    else
      return timing_sense_unknown;
  case op_one:
    return timing_sense_none;
  case op_zero:
    return timing_sense_none;
  }
  // Prevent warnings from lame compilers.
  return timing_sense_unknown;
}

const char *
FuncExpr::asString() const
{
  return asString(false);
}

const char *
FuncExpr::asString(bool with_parens) const
{
  switch (op_) {
  case op_port:
    return port_->name();
  case op_not: {
    const char *left = left_->asString(true);
    size_t left_length = strlen(left);
    size_t length = left_length + 2;
    char *result = makeTmpString(length);
    char *ptr = result;
    *ptr++ = '!';
    strcpy(ptr, left);
    return result;
  }
  case op_or:
    return asStringSubexpr(with_parens, '+');
  case op_and:
    return asStringSubexpr(with_parens, '*');
  case op_xor:
    return asStringSubexpr(with_parens, '^');
  case op_one:
    return "1";
  case op_zero:
    return "0";
  default:
    return "?";
  }
}

const char *
FuncExpr::asStringSubexpr(bool with_parens,
			  char op) const
{
  const char *left = left_->asString(true);
  const char *right = right_->asString(true);
  size_t length = strlen(left) + 1 + strlen(right) + 1;
  if (with_parens)
    length += 2;
  char *result = makeTmpString(length);
  char *r = result;
  if (with_parens)
    *r++= '(';
  stringAppend(r, left);
  *r++ = op;
  stringAppend(r, right);
  if (with_parens)
    *r++ = ')';
  *r = '\0';
  return result;
}

FuncExpr *
FuncExpr::bitSubExpr(int bit_offset)
{
  switch (op_) {
  case op_port:
    if (port_->hasMembers()) {
      if (port_->size() == 1) {
	LibertyPort *port = port_->findLibertyMember(0);
	return makePort(port);
      }
      else {
	LibertyPort *port = port_->findLibertyMember(bit_offset);
	return makePort(port);
      }
    }
    else
      // Always copy so the subexpr doesn't share memory.
      return makePort(port_);
  case op_not:
    return makeNot(left_->bitSubExpr(bit_offset));
  case op_or:
    return makeOr(left_->bitSubExpr(bit_offset),
		  right_->bitSubExpr(bit_offset));
  case op_and:
    return makeAnd(left_->bitSubExpr(bit_offset),
		   right_->bitSubExpr(bit_offset));
  case op_xor:
    return makeXor(left_->bitSubExpr(bit_offset),
		   right_->bitSubExpr(bit_offset));
  case op_one:
  case op_zero:
    return this;
  }
  // Prevent warnings from lame compilers.
  return NULL;
}

bool
FuncExpr::hasPort(const LibertyPort *port) const
{
  switch (op_) {
  case op_port:
    return (port_ == port);
  case op_not:
    return left_ && left_->hasPort(port);
  case op_or:
  case op_and:
  case op_xor:
    return (left_ && left_->hasPort(port))
      || (right_ && right_->hasPort(port));
  case op_one:
  case op_zero:
    return false;
  }
  // Prevent warnings from lame compilers.
  return false;
}

bool
FuncExpr::checkSize(LibertyPort *port)
{
  return checkSize(port->size());
}

bool
FuncExpr::checkSize(size_t size)
{
  size_t port_size;
  switch (op_) {
  case op_port:
    port_size = port_->size();
    return !(port_size == size
	     || port_size == 1);
  case op_not:
    return left_->checkSize(size);
  case op_or:
  case op_and:
  case op_xor:
    return left_->checkSize(size) || right_->checkSize(size);
  case op_one:
  case op_zero:
    return false;
  }
  // Prevent warnings from lame compilers.
  return false;
}

FuncExpr *
funcExprNot(FuncExpr *expr)
{
  if (expr->op() == FuncExpr::op_not) {
    FuncExpr *not_expr = expr->left();
    delete expr;
    return not_expr;
  }
  else
    return FuncExpr::makeNot(expr);
}

////////////////////////////////////////////////////////////////

FuncExprPortIterator::FuncExprPortIterator(FuncExpr *expr)
{
  findPorts(expr);
  iter_.init(ports_);
}

void
FuncExprPortIterator::findPorts(FuncExpr *expr)
{
  if (expr) {
    if (expr->op() == FuncExpr::op_port)
      ports_.insert(expr->port());
    else {
      findPorts(expr->left());
      findPorts(expr->right());
    }
  }
}

bool
FuncExpr::equiv(const FuncExpr *expr1,
		const FuncExpr *expr2)
{
  if (expr1 == NULL && expr2 == NULL)
    return true;
  else if (expr1 != NULL && expr2 != NULL
	   && expr1->op() == expr2->op()) {
    switch (expr1->op()) {
    case FuncExpr::op_port:
      return LibertyPort::equiv(expr1->port(), expr2->port());
    case FuncExpr::op_not:
      return equiv(expr1->left(), expr2->left());
    default:
      return equiv(expr1->left(), expr2->left())
	&& equiv(expr1->right(), expr2->right());
    }
  }
  else
    return false;
}

bool
FuncExpr::less(const FuncExpr *expr1,
	       const FuncExpr *expr2)
{
  if (expr1 != NULL && expr2 != NULL) {
    Operator op1 = expr1->op();
    Operator op2 = expr2->op();
    if (op1 == op2) {
      switch (expr1->op()) {
      case FuncExpr::op_port:
	return LibertyPort::less(expr1->port(), expr2->port());
      case FuncExpr::op_not:
	return less(expr1->left(), expr2->left());
      default:
	if (equiv(expr1->left(), expr2->left()))
	  return less(expr1->right(), expr2->right());
	else
	  return less(expr1->left(), expr2->left());
      }
    }
    else
      return op1 < op2;
  }
  else if (expr1 == NULL && expr2 == NULL)
    return false;
  else
    return (expr1 == NULL && expr2 != NULL);
}

} // namespace
