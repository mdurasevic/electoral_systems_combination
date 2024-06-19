package domain;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Graph implements Comparable<Graph> {

	// static data
	public List<Point> points;
//	private List<Integer> cities;
	public String name;
	private double[] x;
	private double[] y;
	private double[][] weights;
	private boolean[][] edges;
	private int size;

	// initial values that are calculated where the graph is load
	// for the centroid
	public double sumX;
	public double sumY;
	// for the standardDeviation
	public double standardDeviationX;
	public double standardDeviationY;
	// for Predicted distance from the initial node (Pd)
	public Map<Point, Double> maxDistance;
	// for new fitness
	public double totalDistance;

	public Graph(String fileURL) {
		parseInstance(fileURL);
		size = points.size();
		x = new double[size];
		y = new double[size];
		weights = new double[size][size];
		edges = new boolean[size][size];
		totalDistance = 0;
		sumX = 0;
		sumY = 0;
//		cities = new ArrayList<Integer>();
		maxDistance = new HashMap<Point, Double>();
		createGraph();
	}

	private void parseInstance(String fileURL) {
		points = new ArrayList<Point>();
		try {
			FileReader fr = new FileReader(fileURL);
			BufferedReader bf = new BufferedReader(fr);
			String line = "";
			String[] parts;
			boolean readingCoords = false;
			int punto = 0;
			while ((line = bf.readLine()) != null) {
				if (line.contains("EOF"))
					break;
//				if (line.contains("NAME"))
//					System.out.print(line+" ");
//				if (line.contains("EDGE_WEIGHT_TYPE"))
//					System.out.println(line);
				if (!line.contains("NODE_COORD_SECTION") && !readingCoords)
					continue;
				if (line.contains("NODE_COORD_SECTION")) {
					readingCoords = true;
					continue;
				}

				parts = line.trim().replaceAll("\\s+", " ").split(" ");
//				System.out.println(fileURL);
				Point point = new Point(punto, Double.parseDouble(parts[1]), Double.parseDouble(parts[2]));
//				System.out.println(point.getY());
				punto++;
				points.add(point);
			}
			bf.close();
			fr.close();
		} catch (IOException e) {
			System.out.println(fileURL);
			e.printStackTrace();
		}
		name = fileURL.split("/")[fileURL.split("/").length - 1];
	}

	private void createGraph() {
		for (Point point : points) {
			setPointX(point, point.getX());
			setPointY(point, point.getY());
			// compute centroid y standard desviation
			sumX += point.getX();
			sumY += point.getY();
		}
		double distance;
		double max;
		for (Point point : points) {
			max = 0;
			for (Point point2 : points) {
				distance = point.getDistance(point2);
				addEdge(point.getId(), point2.getId(), distance);
				totalDistance += distance;
				if (max < distance)
					max = distance;
			}
			maxDistance.put(point, max);
		}
	}

	// getter / setters
	public int getSize() {
		return size;
	}

	public String getName() {
		return this.name.replace(".tsp", "");
	}

	public double getEdge(Point source, Point target) {
		return weights[source.getId()][target.getId()];
	}

	public void addEdge(int source, int target, double weight) {
		edges[source][target] = true;
		edges[target][source] = true;
		weights[source][target] = weight;
		weights[target][source] = weight;
	}

	public void setPointX(Point node, double x) {
		this.x[node.getId()] = x;
	}

	public void setPointY(Point node, double y) {
		this.y[node.getId()] = y;
	}

	public double getPointX(Point node) {
		return x[node.getId()];
	}

	public double getPointY(Point node) {
		return y[node.getId()];
	}

	public double getTotalCost() {
		return totalDistance;
	}

	@Override
	public boolean equals(Object obj) {
		return name.equals(((Graph) obj).name);
	}

	public double maxDistance(Point city) {
		return maxDistance.get(city);
	}
	
	@Override
	public int compareTo(Graph g) {
		return size > g.size ? 1 : -1;
	}

}
