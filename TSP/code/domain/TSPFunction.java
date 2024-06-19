package domain;

import java.util.ArrayList;
import java.util.List;

public abstract class TSPFunction {

	// for a instance
	public Graph instance;
	// these values represented the actual state of the solution
	protected List<Point> individualRepresentation;
	protected Individual solucion;
	public List<Point> noVisited;
	protected Point actualCity;
	protected Point candidateCity;
	// cost solution
	protected double computedDistance;
	// centroid
	protected double sumXUnVisited;
	protected double sumYUnVisited;
	// standardDeviation
	protected double standardDeviationX;
	protected double standardDeviationY;

	public Individual generateIndividualAccordingADeterministicMethod(Graph instance) {
		generateInitial(instance);
		// builder
		while (!noVisited.isEmpty()) {
			// calculate priority of each city no visited and choose the most priority city
			setCandidateCity(computePrioritiesAndChooseNextCity());
			individualRepresentation.add(candidateCity);
			if (actualCity != null)
				computedDistance += instance.getEdge(actualCity, candidateCity);
			actualCity = candidateCity;
			noVisited.remove(candidateCity);
			recalculateDomainInformation();
		}
		// End city must be same as initial one
		individualRepresentation.add(individualRepresentation.get(0));
		computedDistance += instance.getEdge(actualCity, individualRepresentation.get(0));
		solucion = new Individual(individualRepresentation, computedDistance);
		return solucion;
	}
	
	public Individual generateIndividualAccordingADeterministicMethod(Graph instance, double cota) {
		generateInitial(instance);
		// builder
		while (!noVisited.isEmpty()) {
			// calculate priority of each city no visited and choose the most priority city
			setCandidateCity(computePrioritiesAndChooseNextCity());
			individualRepresentation.add(candidateCity);
			if (actualCity != null)
				computedDistance += instance.getEdge(actualCity, candidateCity);
			if (computedDistance > cota)
				return new Individual(individualRepresentation, Double.MAX_VALUE);
			actualCity = candidateCity;
			noVisited.remove(candidateCity);
			recalculateDomainInformation();
		}
		// End city must be same as initial one
		individualRepresentation.add(individualRepresentation.get(0));
		computedDistance += instance.getEdge(actualCity, individualRepresentation.get(0));
		solucion = new Individual(individualRepresentation, computedDistance);
		return solucion;
	}

	public void generateInitial(Graph instance) {
		this.instance = instance;
		actualCity = null;
		setCandidateCity(null);
		sumXUnVisited = instance.sumX;
		sumYUnVisited = instance.sumY;
		computedDistance = 0;
		noVisited = new ArrayList<Point>(instance.points);
		individualRepresentation = new ArrayList<Point>();
	}

	Point centroid;
	Double distanceCentroidToCandidateCity;
	Double predictedDistance;
	Double distanceLeftEstimation;
	Double standardDeviation;

	public void setCandidateCity(Point candidateCity) {
		this.candidateCity = candidateCity;
		// lazy evaluation
		centroid = null;
		distanceCentroidToCandidateCity = null;
		predictedDistance = null;
		distanceLeftEstimation = null;
		standardDeviation = null;
	}

	public abstract Point computePrioritiesAndChooseNextCity();

	protected void recalculateDomainInformation() { // it is called after a new city is choosed to be visited
		// centroid
		sumXUnVisited -= instance.getPointX(actualCity);
		sumYUnVisited -= instance.getPointY(actualCity);
	}

	// TERMINALS METHODS
	// no time consuming
	public double getDistanceFromCurrent() {
		if (actualCity == null)
			return 0;
		return instance.getEdge(actualCity, candidateCity);
	}

	public double getDistanceFromInitial() {
		if (actualCity == null)
			return 0;
		return computedDistance;
	}

	// these with lazy evalation
	public double distanceCentroidToCandidateCity() {
		if (distanceCentroidToCandidateCity == null)
			distanceCentroidToCandidateCity = centroid(candidateCity).getDistance(candidateCity);
		return distanceCentroidToCandidateCity;
	}
	
	private Point centroid(Point candidateCity) { 
		if (centroid == null) {
			double localComputedCentroidX = (sumXUnVisited - instance.getPointX(candidateCity)) / (noVisited.size() - 1);
			double localComputedCentroidY = (sumYUnVisited - instance.getPointY(candidateCity)) / (noVisited.size() - 1);
			centroid = new Point(localComputedCentroidX, localComputedCentroidY);
		}
		return centroid;
	}
	
	public double predictedDistance() {
		// if actual node is initial or final
		if (predictedDistance == null) {
			if (actualCity == null || noVisited.size() - 1 == 0) // first or last node
				predictedDistance = 0.0;
			else {
				double dmax = instance.maxDistance(candidateCity);
				if (noVisited.size() == (int) (instance.getSize() / 2)) // in the middle
					predictedDistance = dmax;
				else
					predictedDistance = (-4 * dmax / Math.pow(instance.getSize(), 2) * Math.pow(noVisited.size(), 2)) + (4 * dmax / instance.getSize() * noVisited.size()) + 0;
			}
		}
		return predictedDistance;
	}

	public double distanceLeftEstimation() {
		if (distanceLeftEstimation == null) {
			if (actualCity != null) {
				Point currentCentroid = centroid(candidateCity);
				// distance centroid to candidate
				double centroidToCandidate = currentCentroid.getDistance(candidateCity);
				// distance centroid to initial
				double centroidToInitial = currentCentroid.getDistance(individualRepresentation.get(0));
				distanceLeftEstimation = (centroidToCandidate + centroidToInitial) + standardDeviation() * (noVisited.size() - 1);
			}
		}
		return 0;
	}

	private double standardDeviation() {
		if (standardDeviation == null) {
			double localComputedStandardDeviationX = Math.pow(sumXUnVisited - instance.getPointX(candidateCity), 2);
			double localComputedstandardDeviationY = Math.pow( sumYUnVisited - instance.getPointY(candidateCity), 2);
			Point varianza = new Point(localComputedStandardDeviationX, localComputedstandardDeviationY);
			standardDeviation = Math.sqrt(varianza.getDistance(candidateCity) / (noVisited.size() - 1));
		}
		return standardDeviation;
	}

}
