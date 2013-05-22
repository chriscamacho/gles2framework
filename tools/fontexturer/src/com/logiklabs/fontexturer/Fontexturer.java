package com.logiklabs.fontexturer;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GraphicsEnvironment;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.GridLayout;

import java.io.File;

import javax.swing.*;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.filechooser.FileFilter;

public class Fontexturer extends JFrame implements ActionListener, ChangeListener {
	private JComboBox fontCombo;
	private JSpinner fontSpinner,minSpinner;
	private PreviewPanel preview;
	private ButtonGroup bg2;
	
	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				new Fontexturer().setVisible(true);
			}
		});
	}
	
	public Fontexturer() {
		super("Fontexturer");
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		
		GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
		String[] fonts = env.getAvailableFontFamilyNames();
		
		fontCombo = new JComboBox(fonts);
		fontCombo.setSelectedItem("Times New Roman");
		fontCombo.addActionListener(this);
		fontCombo.setRenderer(new FontCellRenderer());
		
		preview = new PreviewPanel();
		preview.setSideLength(256);
		add(preview, BorderLayout.CENTER);
		
		JPanel controls = new JPanel();
		controls.setLayout( new GridLayout(0,2) );
		controls.setPreferredSize(new Dimension(300, 300));
		controls.setBorder(new CompoundBorder(new TitledBorder("Settings"), new EmptyBorder(8, 8, 8, 8)));
		controls.add(new JLabel("Font"));
		controls.add(fontCombo);

		JCheckBox bounds = new JCheckBox("Show glyph boundaries");
		bounds.addActionListener(this);
		controls.add(new JLabel("."));
		controls.add(bounds);
		controls.add(new JLabel("Texture Width"));
		JPanel jp = new JPanel();
		ButtonGroup bg = new ButtonGroup();
		jp.add(makeRadio("128", bg));
		jp.add(makeRadio("256", bg));
		jp.add(makeRadio("512", bg));
		controls.add(jp);
		
		controls.add(new JLabel("Cell Width"));
		JPanel jp2 = new JPanel();
		bg2 = new ButtonGroup();
		jp2.add(makeRadio( "8",bg2));
		jp2.add(makeRadio("16",bg2));
		jp2.add(makeRadio("32",bg2));
		controls.add(jp2);
		
		
		controls.add(new JLabel("Font Size"));
		fontSpinner = new JSpinner(new SpinnerNumberModel(13, 1, 100, 1));
		fontSpinner.addChangeListener(this);
		controls.add(fontSpinner);
		
		controls.add(new JLabel("Start Char"));
		minSpinner = new JSpinner(new SpinnerNumberModel(0,0,128,1));
		minSpinner.addChangeListener(this);
		controls.add(minSpinner);
		
		JButton save = new JButton("Save image...");
		save.addActionListener(this);
		controls.add(save);
		
		add(controls, BorderLayout.EAST);
		
		pack();
		setLocationRelativeTo(null);
	}
	
	private JRadioButton makeRadio(String text, ButtonGroup bg) {
		JRadioButton btn = new JRadioButton(text);
		btn.addActionListener(this);
		bg.add(btn);
		return btn;
	}
	
	private void showSaveDialog() {
		JFileChooser fc = new JFileChooser(System.getProperty("user.dir"));
		fc.setFileFilter(new FileFilter() {
			public boolean accept(File f) {
				String s = f.getName();
				return f.isDirectory() || s.endsWith(".png");
			}

			public String getDescription() {
				return "PNG files (*.png)";
			}
			
		});
		int res = fc.showSaveDialog(this);
		if (res != JFileChooser.APPROVE_OPTION)
			return;
		try {
			preview.save(fc.getSelectedFile(), "png");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public void actionPerformed(ActionEvent e) {
		
		
		//System.out.println(e.getSource());
		if(e.getSource() instanceof JComboBox) {
			preview.setFontName((String) fontCombo.getSelectedItem());
		} else {
			String cmd = e.getActionCommand();
			if(cmd.equals("8") || cmd.equals("16") || cmd.equals("32")) // TODO use properly defined actions... 
				preview.setBoxSize(Integer.parseInt(cmd));
			if(cmd.equals("128") || cmd.equals("256") || cmd.equals("512"))
				preview.setSideLength(Integer.parseInt(cmd));
			else if(cmd.equals("Show glyph boundaries"))
				preview.setShowBounds(((JCheckBox) e.getSource()).isSelected());
			else if(cmd.equals("Save image..."))
				showSaveDialog();
		}
	}

	public void stateChanged(ChangeEvent e) {
		if (e.getSource() == fontSpinner ) 
			preview.setFontSize(((Integer) fontSpinner.getValue()).intValue());
			
		if (e.getSource() == minSpinner )
			preview.setMin(((Integer) minSpinner.getValue()).intValue());
	}
}
