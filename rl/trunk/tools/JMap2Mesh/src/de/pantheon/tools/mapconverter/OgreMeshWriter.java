/*
 * Created on 26.10.2004
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */

package de.pantheon.tools.mapconverter;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import  de.pantheon.tools.Tools;
/**
 * @author LopLop
 *
 * 2004
 */
public class OgreMeshWriter
{
    public static void writeMeshXML( Map map, String outdir, String basename, String[] ignoreTextures )
    {
        ArrayList allTextures = new ArrayList();
        int mesh_count = 1;
     
        // Jede Entity ein einzelnes Mesh
        ArrayList entities = map.getEntities();     
        for( int i=0; i < entities.size(); i++ )
        {
            Entity ent = (Entity)entities.get(i);
            
            // Alles Polygone dieser Entity sammeln
            ArrayList polygons = new ArrayList();
            
            ArrayList brushes = ent.getBrushes();
            for( int j=0; j < brushes.size(); j++ )
            {
                Brush brush = (Brush)brushes.get(j);
                ArrayList arr = brush.getPolygons();
                polygons.addAll( arr );
            }
            
            // Abbruch wenn keine Faces in dieser Entity sind
            if( polygons.size() <= 0)
                break;
            
            StringBuffer stream = new StringBuffer();
            // Faces in Dreiecke umwandeln
            ArrayList triangles = Polygon.toTriangles( polygons );

            // Alle Texturen sammeln
            ArrayList textures = new ArrayList();
            
            for( int j=0; j < triangles.size(); j++ )
            {
                Triangle tri = (Triangle)triangles.get(j);
                String texture = tri.getTexture();
                
                if( !textures.contains( texture ) && !Tools.contains(ignoreTextures, texture ) )
                    textures.add( texture );
                   
                if( !allTextures.contains( texture ) && !Tools.contains(ignoreTextures, texture ) )
                    allTextures.add( texture );
                
            }
            
            if( textures.size() <= 0 )
                break;

            stream.append("<mesh>" + "\n");
            stream.append("\t<submeshes>" + "\n");
            
            // Jede Textur muss ein einzelnes Submesh sein
            for( int j=0; j < textures.size(); j++ )
            {  
                String texture = (String)textures.get(j);

                ArrayList subTriangles = new ArrayList();
                
                for( int k=0; k < triangles.size(); k++ )
                { 
                    Triangle tri = (Triangle)triangles.get( k );
                
                    if( tri.getTexture().equals( texture ) )
                        subTriangles.add( tri );
                }    	
                
                stream.append("\t\t<submesh material=\""+basename+"_"+texture+"\" usesharedvertices=\"false\" use32bitindexes=\"false\">"+"\n");
                stream.append("\t\t\t<faces count=\""+subTriangles.size()+"\">"+"\n");
                
                ArrayList vertices = new ArrayList();
                 // Alle Triangles
                for( int k=0; k < subTriangles.size(); k++ )
                {  
                    Triangle tri = (Triangle)subTriangles.get( k );

                    if( !vertices.contains( tri.getVec1() ) )
                        vertices.add( tri.getVec1() );
                    if( !vertices.contains( tri.getVec2() ) )
                        vertices.add( tri.getVec2() );
                    if( !vertices.contains( tri.getVec3() ) )
                        vertices.add( tri.getVec3() );
                    
                    
                    
                    stream.append("\t\t\t\t<face v1=\""+vertices.lastIndexOf( tri.getVec1() )+"\" v2=\""+vertices.lastIndexOf( tri.getVec2() )+"\" v3=\""+vertices.lastIndexOf( tri.getVec3() )+"\" />"+"\n");  
                }
                
                stream.append("\t\t\t</faces>"+"\n");
                stream.append("\t\t\t<geometry vertexcount=\""+vertices.size()+"\">"+"\n");
                stream.append("\t\t\t\t<vertexbuffer positions=\"true\" normals=\"true\" colours_diffuse=\"false\" texture_coords=\"1\" texture_coord_dimensions_0=\"2\" >"+"\n");
                
                // Alle Vertices der Triangles
                for( int k=0; k < vertices.size(); k++ )
                { 
                    Vertex vert = (Vertex)vertices.get(k);
                    
                    stream.append("\t\t\t\t\t<vertex>"+"\n");
                    stream.append("\t\t\t\t\t\t<position x=\""+vert.getPosition().x()+"\" y=\""+vert.getPosition().y()+"\" z=\""+vert.getPosition().z()+"\" />"+"\n");
                    stream.append("\t\t\t\t\t\t<normal x=\""+(vert.getNormal().x())+"\" y=\""+(vert.getNormal().y())+"\" z=\""+(vert.getNormal().z())+"\" />"+"\n");
                    stream.append("\t\t\t\t\t\t<texcoord u=\""+vert.getU()+"\" v=\""+vert.getV()+"\" />"+"\n");
                    stream.append("\t\t\t\t\t</vertex>"+"\n");
                }
                
                stream.append("\t\t\t\t</vertexbuffer>"+"\n");
                stream.append("\t\t\t</geometry>"+"\n");
                stream.append("\t\t</submesh>"+"\n"); 
                
            }
            
            stream.append("\t</submeshes>"+"\n");    
            stream.append("</mesh>"+"\n");
            
            System.out.println( "File written for Entity "+mesh_count+": "+outdir+"/"+basename+"_"+mesh_count+".xml" );            
            writeToFile( outdir+"/"+basename+"_"+mesh_count+".xml",stream.toString() );
            
            mesh_count++;
        }    
        
        StringBuffer stream = new StringBuffer();
        int numWritten = 0;
        
        for( int i=0; i < allTextures.size(); i++ )
        {
            String textureName = (String) allTextures.get(i);
            TextureManager.Texture tex = TextureManager.getSingleton().loadTexture(textureName);
            
            if( tex != null )
            {
	            stream.append("material "+basename+"_"+textureName+"\n");
	            stream.append("{"+"\n");
	            stream.append("\ttechnique"+"\n");
	            stream.append("\t{"+"\n");
	            stream.append("\t\tpass"+"\n");
	            stream.append("\t\t{"+"\n");
	            stream.append("\t\t\ttexture_unit"+"\n");
	            stream.append("\t\t\t{"+"\n");
	            stream.append("\t\t\t\ttexture textures/"+textureName+"."+tex.getExtension()+"\n");
	            stream.append("\t\t\t\tfiltering anisotropic anisotropic anisotropic"+"\n");
	            stream.append("\t\t\t}"+"\n");
	            stream.append("\t\t}"+"\n");
	            stream.append("\t}"+"\n");
	            stream.append("}"+"\n");
	            numWritten++;
            }
        }
        
        if( numWritten > 0 )
        {
            System.out.println( "File written for "+numWritten+" of "+allTextures.size()+" Texture(s): "+outdir+"/"+basename+".material" );            
            writeToFile( outdir+"/"+basename+".material",stream.toString() );
        }
        else
            System.out.println( "No Material File written. Does your texture-directory exist?" );
    }
    
    private static void writeToFile( String filename, String content)
    {
        File f = new File( filename );    
        
        try
        {
            if( f.exists() )
                f.delete();

            f.createNewFile();
            BufferedWriter buf = new BufferedWriter( new FileWriter(f) );
            buf.write( content );
            buf.close();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
}
