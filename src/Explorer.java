import javax.swing.JComponent;
import java.awt.*;

public class Explorer extends JComponent {
    public double x_coord;
    public double y_coord;

    public Explorer(double x, double y){
        this.x_coord = x;
        this.y_coord = y;
        this.setSize(20, 20); 
        repaint();
    }

    @Override
    protected void paintComponent(Graphics g){
        super.paintComponent(g);
        g.setColor(Color.BLUE);
        double radius = 10.0; 
        g.fillOval((int) x_coord - (int) radius, (int) y_coord - (int) radius, 20, 20);
    }
}
