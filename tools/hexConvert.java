import java.util.*;
import java.io.*;

public class hexConvert {

	public static void main(String[] args) throws IOException {
		
		if(args.length < 1){
			System.out.println("No input file specified");
			System.exit(1);
		}
		
		Scanner in = null;
		DataOutputStream dos = null;
		
		try {
			String outName = args[0].split("\\.")[0];
			
			if(args.length > 1){
				outName = args[1];
			}
			
			dos = new DataOutputStream(new FileOutputStream(outName));
			
		} catch (FileNotFoundException e1) {
			System.out.println("Error open output stream");
			System.exit(1);
		}
		
		try {
			in = new Scanner(new File(args[0]));
		} catch (FileNotFoundException e) {
			System.out.println("Error opening file: " + args[0]);
			System.exit(0);
		}
		
		while(in.hasNext()){
			String next = in.next();

			try{
				int value = Integer.parseInt(next, 16);
				//System.out.printf("0x%04X\n", value);
				dos.writeByte( (byte)((value>>8) & 0xFF) );
				dos.writeByte( (byte)(value & 0xFF) );
			}catch(Exception e){
				System.out.println("unable to parse: " + next);
			}
		}

	}
}
