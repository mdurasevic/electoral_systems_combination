package domain;

public class Point {

	private int id;
	private double x;
	private double y;

	public Point(int punto, double x, double y) {
		this.id = punto;
		this.x = x;
		this.y = y;
	}

	public Point(double x, double y) {
		this.x = x;
		this.y = y;
	}

	public int getId() {
		return this.id;
	}

	public double getX() {
		return this.x;
	}

	public double getY() {
		return this.y;
	}

	@Override
	public boolean equals(Object arg0) {
		return id == ((Point) arg0).id && x == ((Point) arg0).x && y == ((Point) arg0).y;
	}

	public double getDistance(Point destination) {
		return Math.sqrt(Math.pow(this.x - destination.getX(), 2) + Math.pow(this.y - destination.getY(), 2));
	}

//	public double getDistance(double pointX, double pointY) {
//		return Math.sqrt(Math.pow(this.x - pointX, 2) + Math.pow(this.y - pointY, 2));
//	}
}