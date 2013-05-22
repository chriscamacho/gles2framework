package com.logiklabs.fontexturer;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.font.TextLayout;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.JPanel;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;

public class PreviewPanel extends JPanel {
	private static char MIN_CH = '\u0000';
	private static final char MAX_CH = '\u00ff';
	
	private static final Color CHECKER_COL = new Color(220, 220, 220);
	
	private String fontName;
	private int fontSize;
	private Font font;
	
	private BufferedImage fontImage;

	private int sideLength;

	private boolean showBounds;
	private int boxSize=16;
	private int charsPerRow;
	
	public PreviewPanel() {
		this("Times New Roman", 13);
	}
	
	public PreviewPanel(String n, int s) {
		super(new BorderLayout());

		fontName = n;
		fontSize = s;
		font = new Font(fontName, Font.PLAIN, fontSize);
		
		setPreferredSize(new Dimension(524, 524));
		setBorder(new CompoundBorder(new TitledBorder("Preview"),
				new EmptyBorder(8, 8, 8, 8)));
	}
	
	public void setMin(int m) {
		MIN_CH = (char) m;
		updatePreview();
	}
	
	public void setBoxSize(int s) {
		boxSize=s;
		updatePreview();
	}
	
	public void setSideLength(int l) {
		if (l <= 0 || (l & (l - 1)) != 0)
			throw new IllegalArgumentException("length is not a positive power of 2");
		sideLength = l;
		updatePreview();
	}
	
	public void setFontSize(int sz) {
		fontSize = sz;
		font = new Font(fontName, Font.PLAIN, sz);
		updatePreview();
	}
	
	public void setFontName(String name) {
		fontName = name;
		font = new Font(name, Font.PLAIN, fontSize);
		updatePreview();
	}
	
	public void setShowBounds(boolean b) {
		showBounds = b;
		updatePreview();
	}
	
	private void updatePreview() {
		fontImage = new BufferedImage(sideLength, sideLength, BufferedImage.TYPE_INT_ARGB);
		Graphics g = fontImage.getGraphics();
		//int n = sideLength / 16;
		int n = sideLength / boxSize;
		for(int y = 0; y < n; y++) {
			for(int x = 0; x < n; x++) {
				if ((x & 0x1) == 0 ^ (y & 0x1) == 0)
					g.setColor(CHECKER_COL);
				else
					g.setColor(Color.white);
				//g.fillRect(x * 16, y * 16, 16, 16);
				g.fillRect(x * boxSize, y * boxSize, boxSize, boxSize);
			}
		}
		
		drawCharacters(fontImage.getGraphics(), Color.black);
		repaint();
	}
	
	private void drawCharacters(Graphics _g, Color col) {
		Graphics2D g = (Graphics2D) _g;
		g.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
				RenderingHints.VALUE_ANTIALIAS_ON);
		g.setFont(font);
		
		//boxSize = g.getFontMetrics().getHeight();
		
		int off = 0;
		charsPerRow = sideLength / boxSize;
		for (char k = MIN_CH; k <= MAX_CH; k++, off++) {
			TextLayout layout = new TextLayout(String.valueOf(k), font,
					g.getFontRenderContext());
			Rectangle2D rect = layout.getBounds();
			
			int x = (off % charsPerRow) * boxSize;
			int y = (off / charsPerRow) * boxSize;
			
			float cx = 1 - (float) rect.getX();
			float cy = boxSize - g.getFontMetrics().getDescent() - 1;

			g.setColor(col);
			layout.draw(g, x + cx, y + cy);
			
			if(showBounds) {
				rect.setRect(x + cx + rect.getX(), y + cy + rect.getY(),
						rect.getWidth(), rect.getHeight());
				g.setColor(Color.green);
				g.draw(rect);
				
				g.setColor(Color.red);
				g.drawRect(x, y, boxSize, boxSize);
			}
		}
	}
	
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		if(fontImage == null)
			return;
		
		int halfLength = sideLength / 2;
		int x = getWidth() / 2 - halfLength;
		int y = getHeight() / 2 - halfLength;
		
		g.drawImage(fontImage, x, y, this);
		g.drawString(charsPerRow + " characters per row, " + boxSize + " px per character", 5, getHeight() - 5);
	}
	
	public void save(File file, String fmt) throws IOException {
		BufferedImage buf = new BufferedImage(sideLength, sideLength, BufferedImage.TYPE_INT_ARGB);
		drawCharacters(buf.getGraphics(), Color.white);
		ImageIO.write(buf, fmt, file);
	}
}
