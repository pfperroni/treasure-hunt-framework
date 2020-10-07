/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file Position.h
 * @class Position
 * @author Peter Frank Perroni
 * @brief This structure represents one location within 1-dimension of the search space.
 * @details A Position on TH (i.e. one location within one actual dimension in the search space)
 *          can be represented by multiple values, instead of the traditional 1-value dimension
 *          (eg., Fourier-based search space and interval-based search space).
 *          The position is an ordered list with any number of elements, whose type must be
 *          of same basic data type for all elements (eg., double or int).
 *          Therefore, to represent the entire search space,
 *          one Position object for every dimension is required.
 */

#ifndef POSITION_H_
#define POSITION_H_

#include <stdexcept>
#include <string>

template <class P = double, int pSize = 1>
struct Position {
	P internalPosition[pSize];
	int size = pSize;

	void checkCompatibility(Position<P, pSize> *position) {
		if(position == NULL) {
			throw std::invalid_argument("Position can not be empty.");
		}
		if(size != position->size) {
			throw std::invalid_argument(std::string("Position's internal sizes are not compatible [")
										+ std::to_string(size) + " != " + std::to_string(position->size) + "].");
		}
	}

public:
	/**
	 * @brief Operator that assigns the values of a buffer to the list
	 *        that represents the current Position.
	 *
	 * The buffer size must be compatible with this Position.
	 *
	 * @param buffer The source buffer to assign to the list.
	 */
	void operator =(P *buffer) {
		if(buffer == NULL) return;
		for(int i=0; i < size; i++){
			internalPosition[i] = buffer[i];
		}
	}

	/**
	 * @brief Operator that overrides the contents of this position with
	 *        the contents of the Position instance received.
	 * @param position The source Position instance.
	 * @throws invalid_argument if the source Position instance is not compatible with current position.
	 */
	void operator =(Position<P, pSize> *position){
		checkCompatibility(position);
		*this = position->internalPosition;
	}

	/**
	 * @brief Operator that overrides the contents of this position with
	 *        the contents of the Position instance received.
	 * @param position The source Position instance.
	 * @throws invalid_argument if the source Position instance is not compatible with current position.
	 */
	void operator =(Position<P, pSize> &position){
		checkCompatibility(&position);
		*this = position.internalPosition;
	}

	/**
	 * @brief Operator that assigns the same value to all elements of the list
	 *        that represents the current Position.
	 * @param value The value to assign to the list.
	 */
	void operator =(P value) {
		for(int i=0; i < size; i++){
			internalPosition[i] = value;
		}
	}

	/**
	 * @brief Compares the current position with the buffer received.
	 *
	 * The buffer size must be compatible with this Position.
	 *
	 * @param buffer The buffer to compare.
	 * @return True if this Position instance has the same contents as the
	 *         buffer received. False otherwise.
	 */
	bool equals(P *buffer){
		if(buffer == NULL) return false;
		for(int i=0; i < size; i++){
			if(this->internalPosition[i] != buffer[i]) return false;
		}
		return true;
	}

	/**
	 * @brief Compares the current position with the position received.
	 * @param position The Position instance to compare.
	 * @return True if this Position instance has the same contents as the
	 *         position received. False otherwise.
	 */
	bool equals(Position<P, pSize> *position){
		checkCompatibility(position);
		return equals(position->internalPosition);
	}
	bool operator ==(Position<P, pSize> *position){
		return position != NULL && equals(position);
	}
	bool operator ==(Position<P, pSize> &position){
		return position != NULL && equals(&position);
	}
	bool operator !=(Position<P, pSize> *position){
		return position == NULL || ! equals(position);
	}
	bool operator !=(Position<P, pSize> &position){
		return position == NULL || ! equals(&position);
	}

	/**
	 * @brief Math method that sums (individually) each value in the list
	 *        that represents the Position with the value received.
	 * @param value The value to be added to every element in the list that
	 *        represents the Position.
	 */
	void sum(double value) {
		for(int i=0; i < size; i++){
			internalPosition[i] += value;
		}
	}

	/**
	 * @brief Math method that sums the list that represents the Position
	 *        with the values from the buffer received.
	 *
	 * The buffer size must be compatible with this Position.
	 *
	 * @param value The values to be added to the elements in the list that
	 *        represents the Position.
	 */
	void sum(P *buffer) {
		if(buffer == NULL) return;
		for(int i=0; i < size; i++){
			internalPosition[i] += buffer[i];
		}
	}

	/**
	 * @brief Math method that sums the list that represents the Position
	 *        with Position instance received.
	 * @param position The source Position instance.
	 * @throws invalid_argument if the source Position instance is not compatible with current position.
	 */
	void sum(Position<P, pSize> *position) {
		checkCompatibility(position);
		sum(position->internalPosition);
	}

	/**
	 * @brief Math method that sums the list that represents the Position
	 *        with Position instance received.
	 * @param position The source Position instance.
	 * @throws invalid_argument if the source Position instance is not compatible with current position.
	 */
	void sum(Position<P, pSize> &position) {
		sum(&position);
	}

	/**
	 * @brief Math method that subtracts the value received from each value
	 *        in the list that represents the Position (individually).
	 * @param value The value to be subtracted from every element in the list that
	 *        represents the Position.
	 */
	void sub(double value) {
		for(int i=0; i < size; i++){
			internalPosition[i] -= value;
		}
	}

	/**
	 * @brief Math method that subtracts the values of the buffer received from
	 *        the list that represents the Position.
	 *
	 * The buffer size must be compatible with this Position.
	 *
	 * @param buffer The buffer to be subtracted from the elements in the list that
	 *        represents the Position.
	 */
	void sub(P *buffer) {
		if(buffer == NULL) return;
		for(int i=0; i < size; i++){
			internalPosition[i] -= buffer[i];
		}
	}

	/**
	 * @brief Math method that subtracts the list that represents the Position
	 *        from Position instance received.
	 * @param position The source Position instance.
	 * @throws invalid_argument if the source Position instance is not compatible with current position.
	 */
	void sub(Position<P, pSize> *position) {
		checkCompatibility(position);
		sub(position->internalPosition);
	}

	/**
	 * @brief Math method that subtracts the list that represents the Position
	 *        from Position instance received.
	 * @param position The source Position instance.
	 * @throws invalid_argument if the source Position instance is not compatible with current position.
	 */
	void sub(Position<P, pSize> &position) {
		sub(&position);
	}

	/**
	 * @brief Math method that multiplies (individually) each value in the list
	 *        that represents the Position with the value received.
	 *
	 * Notice that any division can be obtained by multiplying by (1/value).
	 *
	 * @param value The value to multiply every element in the list that
	 *        represents the Position.
	 */
	void mult(double value) {
		for(int i=0; i < size; i++){
			internalPosition[i] *= value;
		}
	}

	/**
	 * @brief Math method that multiplies the list that represents the Position
	 *        with the values from the buffer received.
	 *
	 * The buffer size must be compatible with this Position.
	 *
	 * @param value The values to multiply the elements in the list that
	 *        represents the Position.
	 */
	void mult(P *buffer) {
		if(buffer == NULL) return;
		for(int i=0; i < size; i++){
			internalPosition[i] *= buffer[i];
		}
	}

	/**
	 * @brief Math method that multiplies the list that represents the Position
	 *        with Position instance received.
	 * @param position The source Position instance.
	 * @throws invalid_argument if the source Position instance is not compatible with current position.
	 */
	void mult(Position<P, pSize> *position) {
		checkCompatibility(position);
		mult(position->internalPosition);
	}

	/**
	 * @brief Math method that multiplies the list that represents the Position
	 *        with Position instance received.
	 * @param position The source Position instance.
	 * @throws invalid_argument if the source Position instance is not compatible with current position.
	 */
	void mult(Position<P, pSize> &position) {
		mult(&position);
	}

	/**
	 * @brief Limit upper the values of this position.
	 *
	 * Any value that is larger than maxPos will be set to maxPos.
	 *
	 * @param maxPos The upper limit.
	 */
	void adjustUpperBound(P maxPos) {
		for(int i=0; i < size; i++){
			if(internalPosition[i] > maxPos) internalPosition[i] = maxPos;
		}
	}

	/**
	 * @brief Limit lower the values of this position.
	 *
	 * Any value that is smaller than maxPos will be set to maxPos.
	 *
	 * @param maxPos The lower limit.
	 */
	void adjustLowerBound(P minPos) {
		for(int i=0; i < size; i++){
			if(internalPosition[i] < minPos) internalPosition[i] = minPos;
		}
	}

	/**
	 * @brief Get a partial position based on its index in the list of values that
	 *        represents the actual Position.
	 *
	 * Given that a Position can be represented by multiple values, a "partial position"
	 * here actually means one of the values that represents a single dimension.
	 *
	 * @param i The index of the partial position in the list (index list starts in zero).
	 * @return The partial position selected.
	 */
	P getInternalPosition(int i){
		if(i < 0 || i >= pSize) throw std::invalid_argument("Invalid index for internal position.");
		return internalPosition[i];
	}

	/**
	 * @brief Get a pointer to the actual list of values that represents the Position.
	 * @return The pointer to the position's values.
	 */
	P* getInternalPosition(){ return internalPosition; }

	/**
	 * @brief This method copies the contents of current Position to the
	 *        buffer received.
	 *
	 * The buffer size must be compatible with this Position.
	 *
	 * @param buffer The destination buffer.
	 */
	void getInternalPosition(P *buffer){
		if(buffer == NULL) return;
		for(int i=0; i < size; i++){
			buffer[i] = internalPosition[i];
		}
	}

	/**
	 * @brief Get the first value from the list of values that represents the Position.
	 *
	 * Useful when the dimensions are represented by 1 single value.
	 *
	 * @return The first value from the list of values that represents the position.
	 */
	P getFirstValue(){ return internalPosition[0]; }

	/**
	 * @brief Get the lower value from the list of values that represents the Position.
	 * @return The lower value from the list of values that represents the position.
	 */
	P getLowerValue() {
		P lower = internalPosition[0];
		for(int i=1; i < size; i++) {
			if(internalPosition[i] < lower) {
				lower = internalPosition[i];
			}
		}
		return lower;
	}

	/**
	 * @brief Get the upper value from the list of values that represents the Position.
	 * @return The upper value from the list of values that represents the position.
	 */
	P getUpperValue() {
		P upper = internalPosition[0];
		for(int i=1; i < size; i++) {
			if(internalPosition[i] > upper) {
				upper = internalPosition[i];
			}
		}
		return upper;
	}


	/**
	 * @brief Get the sum of values from the list that represents the Position.
	 *
	 * Useful when the dimension is represented by a composite value.
	 *
	 * @return The sum of values from the list that represents the position.
	 */
	P sumInternalValues() {
		P sum = 0;
		for(int i=0; i < size; i++){
			sum += internalPosition[i];
		}
		return sum;
	}

	/**
	 * @brief Get the number of values that represents a Position.
	 * @return The number of values that represents a Position.
	 */
	int getPositionSize() { return size; }
};

#endif /* POSITION_H_ */
